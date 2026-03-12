#ifndef HANDLERS_H
#define HANDLERS_H

void handlePostFile(int client_fd, char *req, char *directory);
void handleGetFile(int client_fd, char *req, char *directory);
void userAgentEndpoint(int client_fd, char *req);
void echoEndpoint(char *response, char *url);

#endif
