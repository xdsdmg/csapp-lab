#include "http.h"

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";
static const char *default_http_port = "80";
static const char eof = '\0';

void parse_uri(char *uri_raw, URI *uri) {
  char *host_pose = strstr(uri_raw, "//");
  if (host_pose == NULL) {
    char *path_pose = strstr(uri_raw, "/");
    if (path_pose != NULL)
      strcpy(uri->path, path_pose);
    strcpy(uri->port, default_http_port);
    return;
  } else {
    char *port_pose = strstr(host_pose + 2, ":");
    if (port_pose != NULL) {
      int port;
      sscanf(port_pose + 1, "%d%s", &port, uri->path);
      sprintf(uri->port, "%d", port);
      *port_pose = eof;
    } else {
      char *path_pose = strstr(host_pose + 2, "/");
      if (path_pose != NULL) {
        strcpy(uri->path, path_pose);
        strcpy(uri->port, default_http_port);
        *path_pose = eof;
      }
    }
    strcpy(uri->host, host_pose + 2);
  }

  return;
}

void parse_header_line(char *header_line, Header *header) {
  int index = 0;

  for (int i = 0; header_line[i] != eof; i++) {
    if (header_line[i] == ':') {
      index = i;
      break;
    }
  }

  strcpy(header->value, header_line + index + 2);

  if (header->value[strlen(header->value) - 2] == '\r' &&
      header->value[strlen(header->value) - 1] == '\n')
    header->value[strlen(header->value) - 2] = eof;
  else if (header->value[strlen(header->value) - 1] == '\n')
    header->value[strlen(header->value) - 1] = eof;

  header_line[index] = eof;

  strcpy(header->name, header_line);
}

void parse_header(Header headers[MAX_HEADER_TOTAL], rio_t *rio) {
  char line[MAXLINE];
  char ua[MAXLINE];

  strcpy(ua, user_agent_hdr);
  parse_header_line(ua, &headers[0]);

  for (int i = 1; (Rio_readlineb(rio, line, MAXLINE) != 0); i++) {
    if (!strcmp(line, "\r\n"))
      break;
    parse_header_line(line, &headers[i]);
  }
}

int parse_request(int conn_fd, Request *req) {
  char method[MAXLINE], uri_str[MAXLINE], version[MAXLINE];

  rio_t rio;
  Rio_readinitb(&rio, conn_fd);
  char request_line[MAXLINE];
  Rio_readlineb(&rio, request_line, MAXLINE);
  sscanf(request_line, "%s %s %s", method, uri_str, version);

  if (strcasecmp(method, "GET") != 0) {
    printf("Proxy does not implement the method %s\n", method);
    return -1;
  }

  URI *uri = (URI *)malloc(sizeof(URI));
  parse_uri(uri_str, uri);
  parse_header(req->headers, &rio);

  req->request_line = request_line;
  req->method = method;
  req->uri = uri;
  req->version = version;

  return 0;
}

void build_request_payload(Request *req, char payload[MAX_PAYLOAD_SIZE]) {
  sprintf(payload, "%s %s %s\r\n", req->method, req->uri->path, req->version);

  for (int i = 0; i < MAX_HEADER_TOTAL && strlen(req->headers[i].name) > 0;
       i++) {
    char line[MAXLINE];
    sprintf(line, "%s: %s\r\n", req->headers[i].name, req->headers[i].value);
    strcat(payload, line);
  }

  strcat(payload, "\r\n");
}

void print_uri(URI *uri_data) {
  printf("[uri] host: %s, port: %s, path: %s\n", uri_data->host, uri_data->port,
         uri_data->path);
}

void print_request(Request *req) {
  printf("[print_request] request_line: %s\nmethod: %s, version: %s\n",
         req->request_line, req->method, req->version);
  print_uri(req->uri);
  for (int i = 0; i < MAX_HEADER_TOTAL && strlen(req->headers[i].name) > 0; i++)
    printf("[print_request] name: %s, value: %s\n", req->headers[i].name,
           req->headers[i].value);
}

void parse_response(int server_fd, rio_t *rio, Response *resp) {
  size_t n;
  char line[MAXLINE];

  Rio_readlineb(rio, line, MAXLINE);
  strcpy(resp->status_line, line);

  /* Parse response header. */
  size_t content_length = 0;
  for (int i = 0; (n = Rio_readlineb(rio, line, MAXLINE)) != 0; i++) {
    if (!strcmp(line, "\r\n"))
      break;
    parse_header_line(line, &resp->headers[i]);
    if (content_length == 0 &&
        !strcasecmp(resp->headers[i].name, "Content-Length")) {
      content_length = atoi(resp->headers[i].value);
    }
  }

  size_t body_size = 0;
  while ((n = Rio_readnb(rio, line, MAXLINE))) {
    for (int i = 0; i < n; i++) {
      resp->body[body_size + i] = (uint8_t)line[i];
    }
    body_size += n;
  }
  resp->body_size = body_size;
}

size_t build_response_payload(Response *resp,
                              uint8_t payload[MAX_PAYLOAD_SIZE]) {
  char payload_[MAXLINE + MAX_HEADER_TOTAL *
                              (MAX_HEADER_NAME_SIZE + MAX_HEADER_VALUE_SIZE)];

  strcpy(payload_, resp->status_line);

  for (int i = 0; i < MAX_HEADER_TOTAL && strlen(resp->headers[i].name) > 0;
       i++) {
    char line[MAXLINE];
    sprintf(line, "%s: %s\r\n", resp->headers[i].name, resp->headers[i].value);
    strcat(payload_, line);
  }

  strcat(payload_, "\r\n");

  size_t payload_size = 0;
  for (int i = 0; payload_[i] != '\0'; i++)
    payload[payload_size++] = (uint8_t)payload_[i];

  for (int i = 0; i < resp->body_size; i++)
    payload[payload_size++] = resp->body[i];

  return payload_size;
}
