#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

static constexpr int BUF_SIZE = 4096;
static const char* FILE_NAME = "test.txt";

HANDLE g_iocp;

/* ---------------- I/O 类型 ---------------- */

enum IO_TYPE { IO_RECV, IO_SEND, IO_FILE };

/* ---------------- 单次 I/O ---------------- */

struct PER_IO {
    OVERLAPPED ol;
    IO_TYPE type;
    WSABUF buf;
    char data[BUF_SIZE];

    PER_IO() : type(IO_RECV) {
        ZeroMemory(&ol, sizeof(ol));
        buf.buf = data;
        buf.len = BUF_SIZE;
    }
};

/* ---------------- 连接状态 ---------------- */

struct PER_CONN {
    SOCKET sock;
    std::string in_buf;

    HANDLE file;
    uint64_t file_offset;

    PER_CONN(SOCKET s) : sock(s), file(INVALID_HANDLE_VALUE), file_offset(0) {}
};

/* ---------------- 文件读取提交 ---------------- */

void PostFileRead(PER_CONN* c, PER_IO* io) {
    DWORD bytes = 0;
    BOOL ok = ReadFile(c->file, io->data, BUF_SIZE, &bytes, &io->ol);

    if (ok) {
        // 同步完成，手动投递完成包
        PostQueuedCompletionStatus(g_iocp, bytes, (ULONG_PTR)c, &io->ol);
    }
    else {
        DWORD err = GetLastError();
        if (err != ERROR_IO_PENDING) {
            CloseHandle(c->file);
            c->file = INVALID_HANDLE_VALUE;
            delete io;
        }
    }
}

/* ---------------- worker ---------------- */

DWORD WINAPI worker(LPVOID) {
    DWORD bytes = 0;
    ULONG_PTR key = 0;
    OVERLAPPED* pol = nullptr;

    while (true) {
        BOOL ok = GetQueuedCompletionStatus(g_iocp, &bytes, &key, &pol, INFINITE);

        auto* io = reinterpret_cast<PER_IO*>(pol);
        auto* c = reinterpret_cast<PER_CONN*>(key);

        /* ---------- 错误处理（关键修复点） ---------- */
        if (!ok) {
            DWORD err = GetLastError();

            // 文件 EOF：不是错误
            if (io && io->type == IO_FILE && err == ERROR_HANDLE_EOF) {
                CloseHandle(c->file);
                c->file = INVALID_HANDLE_VALUE;
                delete io;
                continue;
            }

            // socket 错误：关闭连接
            if (io && io->type != IO_FILE) {
                closesocket(c->sock);
                if (c->file != INVALID_HANDLE_VALUE)
                    CloseHandle(c->file);
                delete io;
                delete c;
                continue;
            }

            delete io;
            continue;
        }

        /* ---------- SOCKET RECV ---------- */
        if (io->type == IO_RECV) {
            if (bytes == 0) {
                closesocket(c->sock);
                delete io;
                delete c;
                continue;
            }

            c->in_buf.append(io->data, bytes);

            while (true) {
                auto pos = c->in_buf.find('\n');
                if (pos == std::string::npos)
                    break;

                std::string line = c->in_buf.substr(0, pos);
                c->in_buf.erase(0, pos + 1);

                if (line == "GETFILE") {
                    c->file = CreateFileA(FILE_NAME, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

                    if (c->file == INVALID_HANDLE_VALUE) {
                        const char* msg = "open file failed\n";
                        auto* w = new PER_IO();
                        w->type = IO_SEND;
                        memcpy(w->data, msg, strlen(msg));
                        w->buf.len = (ULONG)strlen(msg);
                        WSASend(c->sock, &w->buf, 1, NULL, 0, &w->ol, NULL);
                    }
                    else {
                        c->file_offset = 0;
                        CreateIoCompletionPort(c->file, g_iocp, (ULONG_PTR)c, 0);

                        auto* f = new PER_IO();
                        f->type = IO_FILE;
                        f->ol.Offset = 0;
                        f->ol.OffsetHigh = 0;

                        PostFileRead(c, f);
                    }
                }
                else {
                    auto* w = new PER_IO();
                    w->type = IO_SEND;
                    memcpy(w->data, line.data(), line.size());
                    w->data[line.size()] = '\n';
                    w->buf.len = (ULONG)(line.size() + 1);
                    WSASend(c->sock, &w->buf, 1, NULL, 0, &w->ol, NULL);
                }
            }

            ZeroMemory(&io->ol, sizeof(OVERLAPPED));
            io->type = IO_RECV;
            io->buf.len = BUF_SIZE;
            DWORD flags = 0;
            WSARecv(c->sock, &io->buf, 1, NULL, &flags, &io->ol, NULL);
        }

        /* ---------- FILE READ ---------- */
        else if (io->type == IO_FILE) {
            if (bytes == 0) {
                CloseHandle(c->file);
                c->file = INVALID_HANDLE_VALUE;
                delete io;
                continue;
            }

            c->file_offset += bytes;

            auto* w = new PER_IO();
            w->type = IO_SEND;
            memcpy(w->data, io->data, bytes);
            w->buf.len = bytes;
            WSASend(c->sock, &w->buf, 1, NULL, 0, &w->ol, NULL);

            ZeroMemory(&io->ol, sizeof(OVERLAPPED));
            io->ol.Offset = (DWORD)(c->file_offset & 0xffffffff);
            io->ol.OffsetHigh = (DWORD)(c->file_offset >> 32);

            PostFileRead(c, io);
        }

        /* ---------- SOCKET SEND ---------- */
        else if (io->type == IO_SEND) {
            delete io;
        }
    }
}

/* ---------------- main ---------------- */

int main() {
    WSADATA w;
    WSAStartup(MAKEWORD(2, 2), &w);

    SOCKET listenfd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(listenfd, (sockaddr*)&addr, sizeof(addr));
    listen(listenfd, SOMAXCONN);

    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    for (DWORD i = 0; i < si.dwNumberOfProcessors * 2; ++i)
        CreateThread(NULL, 0, worker, NULL, 0, NULL);

    std::cout << "IOCP Proactor server on port 8888\n";

    while (true) {
        SOCKET s = accept(listenfd, NULL, NULL);
        auto* c = new PER_CONN(s);

        CreateIoCompletionPort((HANDLE)s, g_iocp, (ULONG_PTR)c, 0);

        auto* io = new PER_IO();
        io->type = IO_RECV;

        DWORD flags = 0;
        WSARecv(s, &io->buf, 1, NULL, &flags, &io->ol, NULL);
    }
}
