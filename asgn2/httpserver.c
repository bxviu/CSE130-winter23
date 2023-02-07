#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "asgn2_helper_funcs.h"
#include "httpserver.h"

int itoa(int value, char *ptr) {
    int count = 0, temp;
    if (ptr == NULL)
        return 0;
    if (value == 0) {
        *ptr = '0';
        return 1;
    }

    if (value < 0) {
        value *= (-1);
        *ptr++ = '-';
        count++;
    }
    for (temp = value; temp > 0; temp /= 10, ptr++)
        ;
    *ptr = '\0';
    for (temp = value; temp > 0; temp /= 10) {
        *--ptr = temp % 10 + '0';
        count++;
    }
    return count;
}

void get_cmd_args(Listener_Socket *sock, char **arg) {
    //write(1, arg[1], sizeof(arg[1]));
    //char s[10];
    //itoa(atoi(arg[1]), s);
    //write(1, "a\n", sizeof("a"));
    //write(1, s, sizeof(10));
    //int a =
    listener_init(sock, atoi(arg[1]));
    //char s2[2];
    //itoa(a, s2);
    //s2[1] = '\n';
    //if (a == 1) {
    //    write(1, "1\n", sizeof("1"));
    //} else if (a == 0) {
    //    write(1, "0\n", sizeof("0"));
    //}

    //write(1, "bri'ish\n", sizeof("bri'ish"));
    //write(1, &a, sizeof("a"));
    //printf("1%d\n", a);
}

void accept_connection(Listener_Socket *sock) {
    //write(1, "bri'ish\n", sizeof("bri'ish"));
    int a = listener_accept(sock);
    /*if (a > 0) {
        write(1, "accepted\n", sizeof("accepted"));
    } else {
        write(1, "failed\n", sizeof("failed"));
    }*/
    sock->fd = a;
    //char s[2];
    //itoa(a, s);
    //s[1] = '\n';
    //write(1, s, sizeof(s));
    //printf("2%d\n", a);
}

void process_connection(int sock) {
    char buffer[2048];
    //char *s = "\r\n\r\n";
    //read(sock, buffer, 10);
    ssize_t size = 0;
    int read = 0;
    //do {
    read = read_until(sock, buffer, sizeof(buffer), "\r\n\r\n");
    size += read;
    //} while (read > 0);
    //printf("read:%zd\n", size);
    buffer[size + 1] = '\0';
    Request* req = parse(buffer, size);
    int *infile = NULL;
    int start = -1;
    infile = &start;
    //printf("%d", *infile);
    enum StatusCode statcode = handle_request(req, sock, infile);
    //printf("%d", *infile);
    //pass_bytes(sock, *infile, atoi(req->head_f->value));
    if (!strcmp(req->req_l->method,"GET")) {
        handle_response(statcode, sock, *infile);
    } else {
        handle_response(statcode, sock, -1);
    }
    close(*infile);
    //free(req->head_f->key);
    //free(req->head_f->value);
    //free(req->msg_b->body);
    //free(req->req_l->method);
    //free(req->req_l->uri);
    //free(req->req_l->version);
    free(req->req_l);
    free(req->msg_b);
    free(req->head_f);
    free(req);
    /* fprintf(stderr, "Method: %s\nURI: %s\nVersion: %s\n", req.req_l->method, req.req_l->uri,
    req.req_l->version);
    fprintf(stderr, "Key: %s\nValue: %s\n", req.head_f->key, req.head_f->value);
    fprintf(stderr, "Body: %s\n", req.msg_b->body);*/
    //char *str_res = create_response(statcode);
    //
    //enum StatusCode sc = OK;
    //switch (statcode) {
    //    case OK: printf("Okay\n"); break;
    //    case CREATED: printf("CREATed\n"); break;
    //    case BAD_REQUEST: printf("BR\n"); break;
    //    case FORBIDDEN: printf("FORB\n"); break;
    //    case NOT_FOUND: printf("NF\n"); break;
    //    case INTERNAL_SERVER_ERROR: printf("ISE\n"); break;
    //    case NOT_IMPLEMENTED: printf("NOTI\n"); break;
    //    case VERSION_NOT_SUPPORTED: printf("VNS\n"); break;
    //}
    //write_all(sock, str_res, strlen(str_res));
    //pass_bytes(sock, sock, 10);
    //write(1, buffer, sizeof(buffer));
    //write(1, "hi\n", sizeof("hi"));
    //write_all(sock, buffer, size);
}

void close_connection(int sock) {
    shutdown(sock, SHUT_WR);
    //close(sock);
    //printf("|%d|\n", r);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Unfortunate for you, ded");
        return 0;
    }
    //int listening = -1;
    Listener_Socket sockServer = { .fd = socket(AF_INET, SOCK_STREAM, 0) };
    Listener_Socket sockClient = { .fd = socket(AF_INET, SOCK_STREAM, 0) };
    //get_cmd_args(&sock, argv);
    listener_init(&sockServer, atoi(argv[1]));
    while (true) {
    //for (int i = 0; i < 5; i++) {
        //listening 
        //if (listener_init(sock, atoi(argv[1])) == -1) {
        //    printf("error");
        //    break;
        //}
        //printf("1socket:%d\n", sock->fd);
        sockClient.fd = listener_accept(&sockServer);
        //printf("2socket:%d\n", socks->fd);
        //if (listening == 0) {
            //accept_connection(&sock);
            //printf("socket:%d\n", sock.fd);
            //write(1, "br1u'uh\n", sizeof("br1u'uh"));
        if (sockClient.fd > 2) {
            process_connection(sockClient.fd);
            //write(1, "\n|2|\n", sizeof("v-2-"));
            //close_connection(sock->fd);
            
        }
        close_connection(sockClient.fd);
        //close(sock->fd);
        //sockie.fd = socket(AF_INET, SOCK_STREAM, 0);
        //sock = &sockie;
            //listening = -1;
            //write(1, "|3|\n", sizeof("-3-"));
        //sock.fd = socket(AF_LOCAL, SOCK_STREAM, 0);
            //printf("socket:%d\n", socks->fd);
            //get_cmd_args(&sock, argv);
        //}

    }
    return 0;
}
