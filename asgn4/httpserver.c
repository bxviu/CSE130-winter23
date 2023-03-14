// Asgn 2: A simple HTTP server.
// By: Eugene Chou
//     Andrew Quinn
//     Brian Zhao

#include "asgn2_helper_funcs.h"
#include "connection.h"
#include "debug.h"
#include "response.h"
#include "request.h"
#include "queue.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/file.h>

void handle_connection(int);

void handle_get(conn_t *);
void handle_put(conn_t *);
void handle_unsupported(conn_t *);

void *worker();
void audit_update();

pthread_mutex_t creation_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t creation_waiting = PTHREAD_COND_INITIALIZER;
bool creating = false;
queue_t *bounded_buf = NULL;
int req_id = 0;

int main(int argc, char **argv) {
    if (argc < 2) { //default is 4 threads
        warnx("wrong arguments: %s port_num", argv[0]);
        fprintf(stderr, "usage: [-t num_threads] <port>\n");
        return EXIT_FAILURE;
    }
    debug("port:%s\n", argv[1]);

    int num_threads = 0;
    int args = 0;
    opterr = 0;
    size_t port = -1;
    char *endptr = NULL;
    //accepts the optional arguments when the program starts
    while ((args = getopt(argc, argv, "t::")) != -1) {
        debug("ind:%d, %d\n", optind, argc);
        switch (args) {
        case 't':
            if (argc == 3) {
                debug("ind:%d, %s\n", optind, argv[optind]);
                num_threads = 4;
                port = (size_t) strtoull(argv[optind], &endptr, 10);
                break;
            }
            num_threads = atoi(argv[optind]);
            break;
        case '?':
            fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            fprintf(stderr, "usage: [-t num_threads] <port>\n");
            return EXIT_FAILURE;
        default: num_threads = 4;
        }
        if (port != (size_t) -1) {
            break;
        }
        if (optind == 2) {
            port = (size_t) strtoull(argv[optind + 1], &endptr, 10);
        } else {
            fprintf(stderr, "Too many arguments\n");
            fprintf(stderr, "usage: [-t num_threads] <port>\n");
            return EXIT_FAILURE;
        }
        debug("arg:%s, %d\n", argv[optind + 1], optind);
    }
    if (port == (size_t) (-1)) {
        port = (size_t) strtoull(argv[1], &endptr, 10);
    }
    debug("ind:%d, threads:%d\n", optind, num_threads);
    debug("port:%zu\n", port);

    if (argc > 4) {
        fprintf(stderr, "Too many arguments\n");
        fprintf(stderr, "usage: [-t num_threads] <port>\n");
        return EXIT_FAILURE;
    }

    if (endptr && *endptr != '\0') {
        warnx("invalid port number: %s", argv[1]);
        return EXIT_FAILURE;
    }

    if (num_threads < 1) {
        // warnx("invalid thread amount: %d", num_threads);
        // return EXIT_FAILURE;
        num_threads = 4;
    }

    signal(SIGPIPE, SIG_IGN);
    Listener_Socket sock;
    listener_init(&sock, port);

    bounded_buf = queue_new(10);

    pthread_t *workers;
    workers = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&workers[i], NULL, worker, NULL);
    }

    while (1) {
        uintptr_t connfd = listener_accept(&sock);
        if (connfd > 2) {
            queue_push(bounded_buf, (void *) connfd);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(workers[i], NULL);
    }

    queue_delete(&bounded_buf);

    return EXIT_SUCCESS;
}

void *worker() {
    uintptr_t connfd = -1;
    while (1) {
        queue_pop(bounded_buf, (void **) &connfd);
        debug("connfd: %lu\n", connfd);
        if (connfd > 2) {
            handle_connection(connfd);
            close(connfd);
        }
    }
}

void audit_log(conn_t *conn, const Response_t *res, const Request_t *req) {
    const char *oper = request_get_str(req);
    char *uri = conn_get_uri(conn);
    uint16_t status_code = response_get_code(res);
    char *request_id = conn_get_header(conn, "Request-Id");
    if (request_id == NULL) {
        fprintf(stderr, "%s,%s,%d,%d\n", oper, uri, status_code, 0);
    } else {
        fprintf(stderr, "%s,%s,%d,%s\n", oper, uri, status_code, request_id);
    }
}

void handle_connection(int connfd) {

    conn_t *conn = conn_new(connfd);

    const Response_t *res = conn_parse(conn);

    if (res != NULL) {
        conn_send_response(conn, res);
    } else {
        debug("%s", conn_str(conn));
        const Request_t *req = conn_get_request(conn);
        if (req == &REQUEST_GET) {
            handle_get(conn);
        } else if (req == &REQUEST_PUT) {
            handle_put(conn);
        } else {
            handle_unsupported(conn);
        }
    }

    conn_delete(&conn);
}

void handle_get(conn_t *conn) {

    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;
    debug("getting %s", uri);

    pthread_mutex_lock(&creation_lock);
    while (creating) {
        pthread_cond_wait(&creation_waiting, &creation_lock);
    }
    creating = true;
    // What are the steps in here?
    // request_id is here so get doesn't do stuff before its supposed to
    // without this, would deadlock if file didn't exist
    // now it should wait until its turn if multiple request ids
    // if not it will continue on
    // before, if there was a put with req id 0 and a get with req id 1,
    // function would sometimes finish req id 1 first and respond Not Found
    // which is wrong since there is a put with req id 0 that shouldve placed
    // a file
    // if you have more time in the future, maybe you can remove some stuff
    // so these functions dont look so redundant
    char *request_id = conn_get_header(conn, "Request-Id");
    int id = -1;
    if (request_id == NULL) {
        id = 0;
    } else {
        id = atoi(request_id);
    }
    do {
        creating = false;
        pthread_mutex_unlock(&creation_lock);
        pthread_cond_signal(&creation_waiting);
        pthread_mutex_lock(&creation_lock);
        while (creating) {
            pthread_cond_wait(&creation_waiting, &creation_lock);
        }
        creating = true;
    } while ((access(uri, F_OK) != 0) && id > req_id);
    res = &RESPONSE_OK;
    req_id = id;
    // 1. Open the file.
    int fd = open(uri, O_RDONLY, 0666);
    // If  open it returns < 0, then use the result appropriately
    //   a. Cannot access -- use RESPONSE_FORBIDDEN
    //   b. Cannot find the file -- use RESPONSE_NOT_FOUND
    //   c. other error? -- use RESPONSE_INTERNAL_SERVER_ERROR
    // (hint: check errno for these cases)!
    if (fd < 0) {
        debug("%s: %d", uri, errno);
        if (errno == EACCES) {
            res = &RESPONSE_FORBIDDEN;
        } else if (errno == ENOENT) {
            res = &RESPONSE_NOT_FOUND;
        } else {
            // fprintf(stderr, "errrno%d", errno);
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
        }
        creating = false;
        pthread_mutex_unlock(&creation_lock);
        pthread_cond_signal(&creation_waiting);
        goto out;
    }
    flock(fd, LOCK_SH);
    creating = false;
    pthread_mutex_unlock(&creation_lock);
    pthread_cond_signal(&creation_waiting);

    // 2. Get the size of the file.
    // (hint: checkout the function fstat)!
    // Get the size of the file.
    struct stat fd_stat;
    fstat(fd, &fd_stat);
    off_t filesize = fd_stat.st_size;

    // 3. Check if the file is a directory, because directories *will*
    // open, but are not valid.
    // (hint: checkout the macro "S_IFDIR", which you can use after you call fstat!)
    if (S_ISDIR(fd_stat.st_mode)) {
        res = &RESPONSE_FORBIDDEN;
        goto out;
    }

    // 4. Send the file
    // (hint: checkout the conn_send_file function!)
out:
    if (res == &RESPONSE_OK) {
        conn_send_file(conn, fd, filesize);
    } else {
        conn_send_response(conn, res);
    }
    audit_log(conn, res, &REQUEST_GET);
    if (fd > 0) {
        close(fd);
    }
}

void handle_unsupported(conn_t *conn) {
    debug("handling unsupported request");

    // send responses
    conn_send_response(conn, &RESPONSE_NOT_IMPLEMENTED);
    audit_log(conn, &RESPONSE_NOT_IMPLEMENTED, &REQUEST_UNSUPPORTED);
}

void handle_put(conn_t *conn) {

    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;
    debug("handling put request for %s", uri);

    // Check if file already exists before opening it.
    pthread_mutex_lock(&creation_lock);
    while (creating) {
        pthread_cond_wait(&creation_waiting, &creation_lock);
    }
    creating = true;
    // update request_id to the highest current request_id
    char *request_id = conn_get_header(conn, "Request-Id");
    int id = -1;
    if (request_id == NULL) {
        id = 0;
    } else {
        id = atoi(request_id);
    }
    if (req_id < id) {
        req_id = id;
    }
    bool existed = access(uri, F_OK) == 0;
    debug("%s existed? %d", uri, existed);
    // Open the file..
    if (!existed) {
        open(uri, O_CREAT | O_WRONLY, 0600);
        res = &RESPONSE_CREATED;
    }

    int fd = open(uri, O_WRONLY, 0600);
    if (fd < 0) {
        debug("%s: %d", uri, errno);
        if (errno == EACCES || errno == EISDIR || errno == ENOENT) {
            res = &RESPONSE_FORBIDDEN;
        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
        }
        creating = false;
        pthread_mutex_unlock(&creation_lock);
        pthread_cond_signal(&creation_waiting);
        goto out;
    }
    flock(fd, LOCK_EX);
    ftruncate(fd, 0);
    creating = false;
    pthread_mutex_unlock(&creation_lock);
    pthread_cond_signal(&creation_waiting);

    res = conn_recv_file(conn, fd);

    if (res == NULL) {
        res = &RESPONSE_OK;
    }

out:
    conn_send_response(conn, res);
    audit_log(conn, res, &REQUEST_PUT);
    if (fd > 0) {
        // close also releases flock
        close(fd);
    }
}
