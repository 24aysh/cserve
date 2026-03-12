#include "../include/handlers.h"
#include "../include/http.h"
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  signal(SIGCHLD, SIG_IGN); // avoid zombie processes

  printf("Logs from your program will appear here!\n");

  int server_fd;
  socklen_t client_addr_len;
  struct sockaddr_in client_addr;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd == -1) {
    printf("Socket creation failed: %s\n", strerror(errno));
    return 1;
  }

  int reuse = 1;

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEADDR failed: %s\n", strerror(errno));
    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(4221),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s\n", strerror(errno));
    return 1;
  }

  if (listen(server_fd, 5) != 0) {
    printf("Listen failed: %s\n", strerror(errno));
    return 1;
  }

  printf("Waiting for a client to connect...\n");

  client_addr_len = sizeof(client_addr);

  while (1) {
    char *directory = NULL;
    if (argc == 3 && strcmp(argv[1], "--directory") == 0) {
      directory = argv[2];
    }
    int client_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0) {
      perror("accept");
      continue;
    }
    printf("Client connected\n");
    pid_t pid = fork();
    if (pid == 0) { // child process
      close(server_fd);
      char buffer[4096];
      int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
      if (n <= 0) {
        close(client_fd);
        exit(0);
      }
      buffer[n] = '\0';
      int status_code = 404;
      char *url = extractUrl(buffer);
      char response[1024];
      if (!strcmp(url, "/")) {
        status_code = 200;
        buildResp(response, status_code);
        respond(client_fd, response);
      } else if (!strncmp(url, "/echo", 5)) {
        echoEndpoint(response, url);
        respond(client_fd, response);
      } else if (!strcmp(url, "/user-agent")) {
        userAgentEndpoint(client_fd, buffer);
      } else if (!strncmp(url, "/files", 6)) {
        char *header = strstr(buffer, "POST");
        if (header) {
          handlePostFile(client_fd, buffer, directory);
        } else {
          handleGetFile(client_fd, buffer, directory);
        }

      } else {
        buildResp(response, status_code);
        respond(client_fd, response);
      }
      close(client_fd);
      exit(0);
    }

    close(client_fd);
  }

  close(server_fd);

  return 0;
}