#include "chat.h"

int main(int argc, char *argv[]) {
  ChatServer server(2000, 5);
  server.start();
}