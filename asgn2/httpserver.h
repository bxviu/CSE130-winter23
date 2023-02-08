#pragma once
#include <limits.h>

#define BUFFER_SIZE PATH_MAX

enum StatusCode {
    INIT = -10,
    OK = 200,
    CREATED = 201,
    BAD_REQUEST = 400,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    VERSION_NOT_SUPPORTED = 505
};

enum Command { NONE, GET, PUT };

typedef struct {
    char buf[BUFFER_SIZE + 1];
    uint16_t bufsize;
} Result;

typedef struct {
    // Make an *extra* spot for a null terminator!
    char buf[BUFFER_SIZE + 1];
    uint16_t bufsize;
    char *method;
    char *uri;
    char *version;
} Request_Line;

typedef struct {
    // Make an *extra* spot for a null terminator!
    char buf[BUFFER_SIZE + 1];
    uint16_t bufsize;
    // to future me: make a hash map if you want to keep more than content len
    char *key;
    char *value;
} Header_Field;

typedef struct {
    // Make an *extra* spot for a null terminator!
    char buf[BUFFER_SIZE + 1];
    uint16_t bufsize;
    char *body;
} Message_Body;

typedef struct {
    char buf[BUFFER_SIZE + 1];
    uint16_t bufsize;
    char *code;
    char *phrase;
    char *version; //= "HTTP/1.1";
} Status_Line;

typedef struct {
    enum StatusCode statcode;
    Request_Line *req_l;
    Header_Field *head_f;
    Message_Body *msg_b;
} Request;

typedef struct {
    Status_Line *sts_l;
    Header_Field *head_f;
    Message_Body *msg_b;
} Response;

Request *parse(char *r, ssize_t size);

enum StatusCode handle_request(Request *req, int sock, int *infile);

void handle_response(enum StatusCode statcode, int sock, int infile);
