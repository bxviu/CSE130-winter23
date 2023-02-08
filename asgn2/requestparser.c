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
#define PARSE_REQUEST_LINE     "^([a-zA-Z]{0,8}) /([a-zA-Z0-9._]{2,64}) (HTTP/[0-9]*.[0-9]*)\r\n"
#define PARSE_HEADER_FIELD     "^([a-zA-Z0-9.-]{0,128}): ([a-zA-Z0-9.:/*]*)\r\n"
#define PARSE_HEADER_FIELD_END "[a-zA-Z0-9.:/*-]*\r\n\r\n"
//#define PARSE_MESSAGE_BODY "^[a-zA-Z0-9]{0,128}"

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
            regfree(&re);
            return strlen(c->method) + strlen(c->uri) + strlen(c->version) + 5;
        } else {
            c->method = NULL;
            c->uri = NULL;
            c->version = NULL;
            regfree(&re);
            return 0;
        }
        //for (int i = 2; i < 10; i++) {
        //    printf("%s\n", c->buf + matches[i].rm_so);
        //}
    }
    regfree(&re);
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
        int bytes_read = 0;
        hf->buf[hf->bufsize] = 0;
        do {
            rc = regcomp(&re, PARSE_HEADER_FIELD, REG_EXTENDED);
            assert(!rc);
            rc = regexec(&re, (char *) hf->buf + bytes_read, 3, matches, 0);
            //printf("PARSE | %s\n", hf->buf + bytes_read);
            if (rc == 0) {
                hf->key = hf->buf + bytes_read;
                hf->value = hf->buf + matches[2].rm_so + bytes_read;
                hf->key[matches[1].rm_eo] = '\0';
                hf->value[matches[2].rm_eo - matches[2].rm_so] = '\0';
                //printf("GOT | %s: %s\n", hf->key, hf->value);
                //return
                //printf("nums |%lu|%d\n", strlen(hf->key) + strlen(hf->value) + 4,
                //    matches[2].rm_eo - matches[1].rm_so + 2);
                //bytes_read += strlen(hf->key) + strlen(hf->value) + 4;
                bytes_read += matches[2].rm_eo - matches[1].rm_so + 2;
            } /* else {
                hf->key = NULL;
                hf->value = NULL;
                return 0;
            }*/
            regfree(&re);
        } while (rc == 0 && strcmp(hf->key, "Content-Length"));

        do {
            rc = regcomp(&re, PARSE_HEADER_FIELD_END, REG_EXTENDED);
            assert(!rc);
            rc = regexec(&re, (char *) hf->buf + bytes_read, 3, matches, 0);
            //printf("PARSE | %s\n", hf->buf + bytes_read);
            if (rc == 0) {
                //printf("GOT | %s: %s\n", hf->key, hf->value);
                //return
                bytes_read += matches[0].rm_eo - matches[0].rm_so;
            }
            regfree(&re);
        } while (rc == 0);
        //regfree(&re);
        return bytes_read + 2;
    }
    hf->key = NULL;
    hf->value = NULL;
    regfree(&re);
    return 0;
}

static void parse_message_body(Message_Body *mb, char *b, ssize_t size) {
    //regex_t re;
    //regmatch_t matches[3];
    //int rc;
    for (int i = 0; i < size; i++) {
        mb->buf[i] = b[i];
    }
    mb->bufsize = size;
    mb->body = mb->buf;
    //printf("buf: |%s|%lu|%zd\n", mb->buf, sizeof(mb->body), size);
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

Request *parse(char *r, ssize_t size) {
    Request_Line *c = malloc(sizeof(Request_Line));
    Header_Field *hf = malloc(sizeof(Header_Field));
    Message_Body *mb = malloc(sizeof(Message_Body));
    //printf("got:\n%s\n----\n", r);
    //write(STDERR_FILENO, r, size);
    int rln = cmd_parse(c, r, size);
    if (c->method != NULL) {
        int hfn = parse_header_field(hf, r + rln, size - rln);
        parse_message_body(mb, r + hfn + rln, size - hfn - rln);
    }
    Request *req = malloc(sizeof(Request));
    req->req_l = c;
    req->head_f = hf;
    req->msg_b = mb;
    //{ .req_l = c, .head_f = hf, .msg_b = mb };
    return req;
}
