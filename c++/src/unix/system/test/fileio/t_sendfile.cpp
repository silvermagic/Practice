#include "fmt/core.h"
#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
extern "C" {
#include "tlpi_hdr.h"
}

int main(int argc, char *argv[]) {
  if (argc <= 3 || strcmp(argv[1], "--help") == 0)
    usageErr(fmt::format("Usage: {} ip_addrress port_number filename\n",
                         basename(argv[0]))
                 .c_str());

  const char *ip = argv[1];
  int port = atoi(argv[2]);
  const char *filename = argv[3];

  int filefd = open(filename, O_RDONLY);
  assert(filefd > 0);
  struct stat sb;
  if (fstat(filefd, &sb) == -1)
    errExit("fstat");

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

  sendfile(connfd, filefd, 0, sb.st_size);
  close(connfd);
  close(filefd);
  close(sockfd);
  return 0;
}