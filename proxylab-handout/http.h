/* $begin http.h */
#ifndef __HTTP_H__
#define __HTTP_H__

#include "csapp.h"

#define MAX_HEADER_TOTAL 20
#define MAX_HEADER_NAME_SIZE 100
#define MAX_HEADER_VALUE_SIZE 1024
#define MAX_BODY_SIZE 1024 * 100
#define MAX_PAYLOAD_SIZE                                                       \
  (MAXLINE +                                                                   \
   (MAX_HEADER_TOTAL * (MAX_HEADER_NAME_SIZE + MAX_HEADER_VALUE_SIZE)) +       \
   MAX_BODY_SIZE)

typedef struct {
  char host[MAXLINE];
  char port[MAXLINE];
  char path[MAXLINE];
} URI;

typedef struct {
  char name[MAX_HEADER_NAME_SIZE];
  char value[MAX_HEADER_VALUE_SIZE];
} Header;

typedef struct {
  char *method;
  char *version;
  URI *uri;
  char *request_line;
  Header headers[MAX_HEADER_TOTAL];
} Request;

typedef struct {
  char status_line[MAXLINE];
  Header headers[MAX_HEADER_TOTAL];
  uint8_t body[MAX_BODY_SIZE];
  size_t body_size;
} Response;

void parse_uri(char *uri, URI *uri_data);
void parse_header(Header headers[MAX_HEADER_TOTAL], rio_t *rio);
int parse_request(int conn_fd, Request *req);
void build_request_payload(Request *req, char payload[MAX_PAYLOAD_SIZE]);
void parse_header_line(char *header_line, Header *header);
void print_request(Request *req);
void print_uri(URI *uri_data);
void parse_response(int server_fd, rio_t *rio, Response *resp);
size_t build_response_payload(Response *resp, uint8_t payload[MAX_PAYLOAD_SIZE]);

#endif /* __HTTP_H__ */
/* $end http.h */
