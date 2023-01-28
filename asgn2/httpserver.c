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
    if (a > 0) {
        write(1, "accepted\n", sizeof("accepted"));
    } else {
        write(1, "failed\n", sizeof("failed"));
    }
    sock->fd = a;
    //char s[2];
    //itoa(a, s);
    //s[1] = '\n';
    //write(1, s, sizeof(s));
    //printf("2%d\n", a);
}

void process_connection(int sock) {
    char buffer[100];
    char *s = "\r\n";
    //read(sock, buffer, 10);
    ssize_t size = read_until(sock, buffer, sizeof(buffer), s);
    //printf("read:%zd\n", size);
    buffer[size + 1] = '\0';
    Request req = parse(buffer, size);
    fprintf(stderr, "Method: %s\nURI: %s\nVersion: %s\n", req.req_l->method, req.req_l->uri,
        req.req_l->version);
    fprintf(stderr, "Key: %s\nValue: %s\n", req.head_f->key, req.head_f->value);
    fprintf(stderr, "Body: %s\n", req.msg_b->body);
    Status_Line sl = { .version = "HTTP/1.1", .code = "200", .phrase = "OK" };
    Header_Field hf = { .key = "Content-Length", .value = "3" };
    Message_Body mb = { .body = "OK\n" };
    Response res = { .sts_l = &sl, .head_f = &hf, .msg_b = &mb };
    char *str_res = strdup(res.sts_l->version);
    strcat(
        strcat(
            strcat(strcat(strcat(strcat(strcat(strcat(strcat(strcat(str_res, " "), res.sts_l->code),
                                                   " "),
                                            res.sts_l->phrase),
                                     "\r\n"),
                              res.head_f->key),
                       ": "),
                res.head_f->value),
            "\r\n\r\n"),
        res.msg_b->body);
    write_all(sock, str_res, strlen(str_res));
    //pass_bytes(sock, sock, 10);
    //write(1, buffer, sizeof(buffer));
    //write(1, "hi\n", sizeof("hi"));
    //write_all(sock, buffer, size);
}

void close_connection(int sock) {
    shutdown(sock, SHUT_WR);
}

//struct Request { }; // holds parsed data
//
//int open_file(..); // returns a valid file fd
//
//void handle_put_request(Request *request)
//
//Request parse_data(char *buffer); // parses buffer to request

// void main();
// accept new connection
// read all bytes
// request = parse_data
// open_file
// call your handlers

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Unfortunate for you, ded");
        return 0;
    }
    Listener_Socket sock = { .fd = socket(AF_LOCAL, SOCK_STREAM, 0) };

    get_cmd_args(&sock, argv);
    //while (true) {
    accept_connection(&sock);
    //printf("socket:%d\n", sock.fd);
    //write(1, "br1u'uh\n", sizeof("br1u'uh"));
    process_connection(sock.fd);
    //write(1, "\n|2|\n", sizeof("v-2-"));
    close_connection(sock.fd);
    //write(1, "|3|\n", sizeof("-3-"));
    //}
    return 0;
}
