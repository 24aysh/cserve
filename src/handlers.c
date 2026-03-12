#include "../include/handlers.h"
#include "../include/http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void handlePostFile(int client_fd, char *req, char *directory) {
  char *fname = extractUrl(req) + strlen("/files/");
  char fullpath[512];
  snprintf(fullpath, sizeof(fullpath), "%s%s", directory, fname);

  FILE *fp = fopen(fullpath, "wb");
  if (!fp) {
    respond(client_fd, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
    return;
  }
  char *cl = strstr(req, "Content-Length:");
  int len = 0;
  if (cl) {
    cl += strlen("Content-Length: ");
    len = atoi(cl);
  }
  char *body = strstr(req, "\r\n\r\n");
  if (body)
    body += 4;
  fwrite(body, 1, len, fp);
  fclose(fp);
  respond(client_fd, "HTTP/1.1 201 Created\r\n\r\n");
}

void handleGetFile(int client_fd, char *req, char *directory) {
  char *fname = extractUrl(req) + 7;
  char fullpath[512];
  snprintf(fullpath, sizeof(fullpath), "%s%s", directory, fname);

  FILE *fp = fopen(fullpath, "rb");
  if (!fp) {
    respond(client_fd, "HTTP/1.1 404 Not Found\r\n\r\n");
    return;
  }
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  rewind(fp);
  char *content = malloc(size);
  fread(content, 1, size, fp);
  fclose(fp);

  char header[256];
  snprintf(header, sizeof(header),
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: application/octet-stream\r\n"
           "Content-Length: %d\r\n"
           "\r\n",
           size);

  send(client_fd, header, strlen(header), 0);
  send(client_fd, content, size, 0);

  free(content);
}

void userAgentEndpoint(int client_fd, char *req) {
  char *ua = strstr(req, "User-Agent: ");
  if (!ua) {
    respond(client_fd, "HTTP/1.1 404 Not Found\r\n\r\n");
    return;
  }

  ua += strlen("User-Agent: ");
  char *end = strstr(ua, "\r\n");

  int len = end - ua;

  char user_agent[256];
  memcpy(user_agent, ua, len);
  user_agent[len] = '\0';

  char resp[512];
  snprintf(resp, sizeof(resp),
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: %d\r\n"
           "\r\n"
           "%s",
           len, user_agent);

  respond(client_fd, resp);
}

void echoEndpoint(char *response, char *url) {
  char *start = strchr(url, '/') + 1;
  char *final = strchr(start, '/');

  int len = strlen(final + 1);

  snprintf(response, 1024,
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/plain\r\n"
           "Content-Length: %d\r\n"
           "\r\n"
           "%s",
           len, final + 1);
}
