#ifndef HTTP_H
#define HTTP_H

char *extractUrl(char buf[]);
void respond(int client_fd, char *resp);
void buildResp(char *response, int status_code);

#endif
