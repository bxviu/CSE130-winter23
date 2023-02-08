#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "asgn2_helper_funcs.h"
#include "httpserver.h"

//int writeCommand(int infile, char *buffer, int bytes_written, int bytes_read) {
//    do {
//        int bytes = write(infile, &buffer[bytes_written], bytes_read - bytes_written);
//        if (bytes < 0) {
//            return 1;
//        }
//        bytes_written += bytes;
//    } while (bytes_written < bytes_read);
//    return 0;
//}
//
extern int errno;

int is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

enum StatusCode method_put(Request *req, int sock, int *infile) {
    //if URI exists
    enum StatusCode statcode = INTERNAL_SERVER_ERROR;
    if (access(req->req_l->uri, F_OK) == 0) {
        statcode = OK;
    } else {
        statcode = CREATED;
    }
    *infile = open(req->req_l->uri, O_TRUNC | O_WRONLY | O_CREAT, 0666);
    if (*infile == -1) {
        statcode = BAD_REQUEST;
        //return cleanupReturn(1, filename, command, infile);
    }
    int bytes_written = 0;
    //printf("sizes: %lu, %d", strlen(req->msg_b->body), atoi(req->head_f->value));
    if (req->msg_b->bufsize
        == 0) { //(req->msg_b->body && (int) strlen(req->msg_b->body)) != atoi(req->head_f->value)) {
        bytes_written = pass_bytes(sock, *infile, atoi(req->head_f->value));
    } else {
        bytes_written = write_all(*infile, req->msg_b->body, atoi(req->head_f->value));
    }
    if (bytes_written == -1) {
        statcode = BAD_REQUEST;
        // return cleanupReturn(1, filename, command, infile);
    }
    return statcode;
}

enum StatusCode method_get(Request *req, int *infile) {
    //if URI exists
    char *filename = req->req_l->uri;
    enum StatusCode statcode = INTERNAL_SERVER_ERROR;
    if (access(filename, F_OK)
        == 0) { //https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
        statcode = OK;
    } else {
        statcode = NOT_FOUND;
    }
    if (statcode == OK) {
        *infile = open(filename, O_RDONLY, 0666);
        if (*infile > 0 && !is_regular_file(filename)) {
            statcode = FORBIDDEN;
        }
    }
    //if (*infile == -1 ){//|| (*infile > 0 && !is_regular_file(filename))) {
    //    statcode = INTERNAL_SERVER_ERROR;
    //    int errnum = errno;
    //    fprintf(stderr, "Error opening file: %s|%d\n", strerror(errnum), errnum);
    //    //write(STDERR_FILENO, filename, sizeof(filename));
    //    //return cleanupReturn(1, filename, command, infile);
    //}
    //int bytes_written = 0;
    //do {
    //    bytes_written = pass_bytes(*infile, 1, 100);
    //} while (bytes_written > 0);
    //if (bytes_written == -1) {
    //    write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
    //}
    //free(filename);
    return statcode;
    //do {
    //    bytes_read = read(infile, buffer, sizeof(buffer));
    //    if (bytes_read < 0) {
    //        write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
    //        //return cleanupReturn(1, filename, command, infile);
    //    } else if (bytes_read > 0) {
    //        if (writeCommand(STDOUT_FILENO, buffer, 0, bytes_read)) {
    //            write(STDERR_FILENO, "Operation Failed\n", sizeof("Operation Failed"));
    //            //return cleanupReturn(1, filename, command, infile);
    //        }
    //    }
    //} while (bytes_read > 0);
    //return cleanupReturn(0, filename, command, infile);
}

enum Command get_command(char *method) {
    //printf("%s", method);
    if (!strcmp(method, "GET")) {
        return GET;
    }
    if (!strcmp(method, "PUT")) {
        return PUT;
    }
    return NONE;
}

enum StatusCode handle_request(Request *req, int sock, int *infile) {
    //fprintf(stderr, "Method: %s\nURI: %s\nVersion: %s\n", req->req_l->method, req->req_l->uri,
    //    req->req_l->version);
    //fprintf(stderr, "Key: %s\nValue: %s\n", req->head_f->key, req->head_f->value);
    //fprintf(stderr, "Body: %s|\n", req->msg_b->body);
    //printf("%s", req->req_l->method);
    //printf("%s", req->req_l->method);
    //printf("%s", req->req_l->method);
    enum StatusCode methodresult = req->statcode;
    if (methodresult != BAD_REQUEST) {
        if (!strcmp(req->req_l->version, "HTTP/1.1")) {
            if (req->req_l->method != NULL) {
                enum Command method = get_command(req->req_l->method);
                switch (method) {
                case PUT: methodresult = method_put(req, sock, infile); break;
                case GET: methodresult = method_get(req, infile); break;
                case NONE: methodresult = NOT_IMPLEMENTED; break;
                }
            }
        } else if (strlen(req->req_l->version) == 8) {
            methodresult = VERSION_NOT_SUPPORTED;
        } else {
            methodresult = BAD_REQUEST;
        }
    } else {
        methodresult = BAD_REQUEST;
    }
    return methodresult;
}
