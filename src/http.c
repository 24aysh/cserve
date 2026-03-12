#include "../include/http.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void respond(int client_fd, char *resp) {
  send(client_fd, resp, strlen(resp), 0);
  printf("Responded\n");
}

void buildResp(char *response, int status_code) {
  if (status_code == 200) {
    snprintf(response, 1024, "HTTP/1.1 200 OK\r\n\r\n");
  } else {
    snprintf(response, 1024, "HTTP/1.1 404 Not Found\r\n\r\n");
  }
}

char *extractUrl(char buf[]) {
  static char url[256];
  char *start = strchr(buf, ' ') + 1;
  char *end = strchr(start, ' ');
  int len = end - start;
  memcpy(url, start, len);
  url[len] = '\0';
  return url;
}
