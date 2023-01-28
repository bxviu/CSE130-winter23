#include <assert.h>
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


#include "httpserver.h"

/**
 *  This parses any line that does the following:
 * (1) starts with a word that uses any number of a-zA-Z characters.
 * (2) contains a space
 * (3) has a word that starts with a '/' and contains any number of
       characters in [a-ZA-Z0-9._].
 * (4) has a newline
 */

//#define PARSE_REGEX "^([a-zA-Z]{0,8}) /([a-zA-Z0-9._]{0,20})\n"
//#define PARSE_REGEX "^(([a-zA-Z]{0,8}) /([a-zA-Z0-9._]{2,64}) HTTP/[0-9]{1}.[0-9]{1}){0,2048}\r\n(([a-zA-Z0-9]{0,128}):([a-zA-Z0-9]*)\r\n)?\r\n)[a-zA-Z0-9]{0,128}"
#define PARSE_REQUEST_LINE "^([a-zA-Z]{0,8}) /([a-zA-Z0-9._]{2,64}) (HTTP/[0-9]{1}.[0-9]{1})\r\n"
#define PARSE_HEADER_FIELD "^([a-zA-Z0-9-]{0,128}): ([a-zA-Z0-9]*)\r\n\r\n"
//#define PARSE_MESSAGE_BODY "^[a-zA-Z0-9]{0,128}"

void cmd_dump(Request_Line *c) {
    if (c->method && c->uri && c->version)
        fprintf(stderr, "Method: %s\nURI: %s\nVersion: %s\n", c->method, c->uri, c->version);
    else
        fprintf(stderr, "Command not parsed!\n");
}

//static ssize_t read_bytes(int fd, char buf[], size_t nbytes) {
//    size_t total = 0;
//    ssize_t bytes = 0;
//
//    do {
//        bytes = read(fd, buf + total, nbytes - total);
//        if (bytes < 0) {
//            return bytes;
//        }
//        total += bytes;
//    } while (bytes > 0 && total < nbytes);
//
//    return total;
//}

static int cmd_parse(Request_Line *c, char *b, ssize_t size) {

    regex_t re;
    regmatch_t matches[4];
    int rc;

    // read bytes and set buffersize to 0.
    for (int i = 0; i < size; i++) {
        c->buf[i] = b[i];
    }
    c->bufsize = size;
    if (c->bufsize > 0) {

        // Never forget to null terminate your string!!
        c->buf[c->bufsize] = 0;

        // First step -- compile the regex. If this fails, there's
        // probably an issue with your PARSE_REGEX string
        rc = regcomp(&re, PARSE_REQUEST_LINE, REG_EXTENDED);
        assert(!rc);

        // Next step -- use the regex on the string.
        // The parameters are
        // (1) the regex,
        // (2) the buffer to search,
        // (3) the number of submatches within the string, plus 1
        // (4) a regexmatch_t to store the submatches
        // (5) options (see the man page for regex)

        // returns 0 when a match is found.  Assigns the i^th submatch to
        // matches[i] (where we index from `1').  Each element in matches
        // includes:
        // (1) rm_so: a start offset of where in buf the match starts
        // (2) rm_eo: an end offset of where in buf the match terminates.
        rc = regexec(&re, (char *) c->buf, 4, matches, 0);

        if (rc == 0) {
            c->method = c->buf;
            c->uri = c->buf + matches[2].rm_so;
            c->version = c->buf + matches[3].rm_so;
            // Uncomment me to fixup issues in the above!
            c->method[matches[1].rm_eo] = '\0';
            c->uri[matches[2].rm_eo - matches[2].rm_so] = '\0';
            c->version[matches[3].rm_eo - matches[3].rm_so] = '\0';
            return strlen(c->method) + strlen(c->uri) + strlen(c->version) + 5;
        } else {
            c->method = NULL;
            c->uri = NULL;
            c->version = NULL;
            return 0;
        }
        //for (int i = 2; i < 10; i++) {
        //    printf("%s\n", c->buf + matches[i].rm_so);
        //}
    }
    return 0;
}

static int parse_header_field(Header_Field *hf, char *b, ssize_t size) {
    regex_t re;
    regmatch_t matches[3];
    int rc;
    for (int i = 0; i < size; i++) {
        hf->buf[i] = b[i];
    }
    hf->bufsize = size;
    //printf("1: %s\n%d\n", hf->buf, hf->bufsize);
    if (hf->bufsize > 0) {
        hf->buf[hf->bufsize] = 0;
        rc = regcomp(&re, PARSE_HEADER_FIELD, REG_EXTENDED);
        assert(!rc);
        rc = regexec(&re, (char *) hf->buf, 3, matches, 0);
        if (rc == 0) {
            hf->key = hf->buf;
            hf->value = hf->buf + matches[2].rm_so;
            hf->key[matches[1].rm_eo] = '\0';
            hf->value[matches[2].rm_eo - matches[2].rm_so] = '\0';
            //printf("2: %s\n", hf->key);
            return strlen(hf->key) + strlen(hf->value) + 6;
        } else {
            hf->key = NULL;
            hf->value = NULL;
            return 0;
        }
    }
    return 0;
}

static void parse_message_body(Message_Body *mb, char *b, ssize_t size) {
    //regex_t re;
    //regmatch_t matches[3];
    //int rc;
    for (int i = 0; i < size; i++) {
        mb->buf[i] = b[i];
    }
    //printf("%s\n", mb->buf);
    mb->bufsize = size;
    mb->body = mb->buf;
    //if (mb->bufsize > 0) {
    //    mb->buf[mb->bufsize] = 0;
    //    rc = regcomp(&re, PARSE_HEADER_FIELD, REG_EXTENDED);
    //    assert(!rc);
    //    rc = regexec(&re, (char *) mb->buf, 3, matches, 0);
    //    if (rc == 0) {
    //        mb->key = mb->buf;
    //        mb->value = mb->buf + matches[2].rm_so;
    //        mb->key[matches[1].rm_eo] = '\0';
    //        mb->value[matches[2].rm_eo - matches[2].rm_so] = '\0';
    //        //return strlen(mb->key) + strlen(mb->value) + 1;
    //    } else {
    //        mb->key = NULL;
    //        mb->value = NULL;
    //        //return 0;
    //    }
    //}
    //return 0;
}

void print_usage(char *exec) {

    printf("usage: %s\n\n", exec);
    printf("Parses a get/set command header that were passed to stdin\n");
    printf("The format for valid a command is:\n");
    printf("\t[method] [location]\\n\n");
    printf("where:\n");
    printf("\t[method] has between 1 and 8 characters in the range [a-zA-Z]\n");
    printf("\t[location] starts with '/' and includes up to 64 characters ");
    printf("in the set [a-zA-Z0-9._]\n");
}

Request parse(char *r, ssize_t size) {
    Request_Line c;
    (void) c;
    printf("got:\n%s\n----\n", r);
    int rln = cmd_parse(&c, r, size);
    Header_Field hf;
    int hfn = parse_header_field(&hf, r+rln, size-rln);
    Message_Body mb;
    parse_message_body(&mb, r+hfn+rln, size-hfn-rln);
    //cmd_dump(&c);

    Request req = { .req_l = &c, .head_f = &hf, .msg_b = &mb };
    return req;
}
