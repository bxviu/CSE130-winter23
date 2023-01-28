#pragma once
#include <limits.h>

#define BUFFER_SIZE PATH_MAX

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
    Request_Line *req_l;
    Header_Field *head_f;
    Message_Body *msg_b;
} Request;

typedef struct {
    Status_Line *sts_l;
    Header_Field *head_f;
    Message_Body *msg_b;
} Response;



Request parse(char *r, ssize_t size);
