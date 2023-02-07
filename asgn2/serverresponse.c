#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "asgn2_helper_funcs.h"
#include "httpserver.h"

long get_file_size(int filename) {
    struct stat file_status;
    if (fstat(filename, &file_status) < 0) {
        return -1;
    }
    return file_status.st_size;
}

int send_response(Response *res, int sock, int infile) {
    //char *str_res = strdup(res->sts_l->version);
    char str_res[100]; 
    snprintf(str_res, sizeof(str_res), "%s %s %s\r\n%s: ", res->sts_l->version,
        res->sts_l->code, res->sts_l->phrase, res->head_f->key);
    int read_bytes = 0;
    if (!strcmp(res->head_f->value, "?")) {
        //write_all(sock, str_res, strlen(str_res)); 
        long g = get_file_size(infile);
        //char str[100];
        sprintf(str_res + strlen(str_res), "%ld\r\n\r\n", g);
        //str[5] = '\0';
        //strcat(str, "\r\n\r\n");
        write_all(sock, str_res, strlen(str_res)); 
        int read_bytes = pass_bytes(infile, sock, g); 
        //printf("%d %d\n", r, infile);
        return read_bytes;
    } else {
        sprintf(str_res + strlen(str_res), "%s\r\n\r\n%s", res->head_f->value, res->msg_b->body);
        read_bytes = write_all(sock, str_res, strlen(str_res));
        return read_bytes;
        
    }
}

Response *create_response(enum StatusCode statcode, int infile) {
    Status_Line *sl = malloc(sizeof(Status_Line)); //{ .version = "HTTP/1.1", .code = "200", .phrase = "OK" };
    Header_Field *hf = malloc(sizeof(Header_Field)); //{ .key = "Content-Length", .value = "3" };
    Message_Body *mb = malloc(sizeof(Message_Body)); //{ .body = "OK\n" };
    
    sl->version = "HTTP/1.1";
    hf->key = "Content-Length";
    switch (statcode) {
        case OK: 
            sl->code = "200";
            sl->phrase = "OK";
            if (infile > 2) {
                hf->value = "?";
                mb->body = "?";
            } else {
                hf->value = "3";
                mb->body = "OK\n";
            }
            break;
        case CREATED: 
            sl->code = "201";
            sl->phrase = "Created";
            hf->value = "8";
            mb->body = "Created\n"; 
            break;
        case BAD_REQUEST: 
            sl->code = "400";
            sl->phrase = "Bad Request";
            hf->value = "12";
            mb->body = "Bad Request\n";
            break;
        case FORBIDDEN: 
            sl->code = "403";
            sl->phrase = "Forbidden";
            hf->value = "10";
            mb->body = "Forbidden\n";
            break;
        case NOT_FOUND: 
            sl->code = "404";
            sl->phrase = "Not Found";
            hf->value = "10";
            mb->body = "Not Found\n";
            break;
        case INTERNAL_SERVER_ERROR: 
            sl->code = "500";
            sl->phrase = "Internal Server Error";
            hf->value = "22";
            mb->body = "Internal Server Error\n"; 
            break;
        case NOT_IMPLEMENTED: 
            sl->code = "501";
            sl->phrase = "Not Implemented";
            hf->value = "16";
            mb->body = "Not Implemented\n";
            break;
        case VERSION_NOT_SUPPORTED: 
            sl->code = "505";
            sl->phrase = "Version Not Supported";
            hf->value = "22";
            mb->body = "Version Not Supported\n";
            break;
    }
    Response *res = malloc(sizeof(Response)); //{ .sts_l = &sl, .head_f = &hf, .msg_b = &mb };
    res->sts_l = sl;
    res->head_f = hf;
    res->msg_b = mb;
    return res;
}

void handle_response(enum StatusCode statcode, int sock, int infile) {
    Response *res = create_response(statcode, infile);
    send_response(res, sock, infile);
    //free everything
    //free(res->head_f->key);
    //free(res->head_f->value);
    //free(res->msg_b->body);
    //free(res->sts_l->phrase);
    //free(res->sts_l->code);
    //free(res->sts_l->version);
    free(res->sts_l);
    free(res->msg_b);
    free(res->head_f);
    free(res);
}
