#include "fmt/core.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
extern "C" {
#include "tlpi_hdr.h"
}

int main(int argc, char *argv[]) {
  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr(
        fmt::format("Usage: {} ip_addrress port_number\n", basename(argv[0]))
            .c_str());

  const char *ip = argv[1];
  int port = atoi(argv[2]);

  struct sockaddr_in srv_addr;
  bzero(&srv_addr, sizeof(srv_addr));
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(port);
  inet_pton(AF_INET, ip, &srv_addr.sin_addr);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
    errExit("socket");

  int ret = bind(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
  if (ret == -1)
    errExit("bind");

  ret = listen(sockfd, 5);
  if (ret == -1)
    errExit("listen");

  struct sockaddr_in cli_addr;
  socklen_t cli_addr_len = sizeof(cli_addr);
  int connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_addr_len);
  if (connfd == -1)
    errExit("accept");

  int pipefd[2];
  if (pipe(pipefd) == -1)
    errExit("pipe");

  ret = splice(connfd, NULL, pipefd[1], NULL, 32768,
               SPLICE_F_MORE | SPLICE_F_MOVE);
  if (ret == -1)
    errExit("splice");
  ret = splice(pipefd[0], NULL, connfd, NULL, 32768,
               SPLICE_F_MORE | SPLICE_F_MOVE);
  if (ret == -1)
    errExit("splice");
  close(connfd);
  close(pipefd[0]);
  close(pipefd[1]);
  close(sockfd);
  return 0;
}