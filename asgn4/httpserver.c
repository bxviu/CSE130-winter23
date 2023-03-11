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

void *worker();//(void *);
void audit_update();

pthread_mutex_t creation_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t creation_waiting = PTHREAD_COND_INITIALIZER;
bool creating = false; 
queue_t *bounded_buf = NULL;

int main(int argc, char **argv) {
    if (argc < 2) { //default is 4 threads
        warnx("wrong arguments: %s port_num", argv[0]);
        fprintf(stderr, "usage: [-t num_threads] <port>\n");
        return EXIT_FAILURE;
    }
    // debug("port:%s\n", argv[1]);

    int num_threads = 0; 
    int args = 0;
    opterr = 0;
    size_t port = -1;
    char *endptr = NULL;
    while ((args = getopt(argc, argv, "t::")) != -1) {
        debug("ind:%d\n", optind);
        switch (args) {
            case 't':
                if (optind >= argc) {
                    fprintf(stderr, "Expected argument after options\n");
                    fprintf(stderr, "usage: [-t num_threads] <port>\n");
                    return EXIT_FAILURE;
                }
                num_threads = atoi(argv[optind]);
            break;
            case '?':
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                fprintf(stderr, "usage: [-t num_threads] <port>\n");
                return EXIT_FAILURE;
            default:
                num_threads = 4;
        }
        if (optind == 2) {
            port = (size_t) strtoull(argv[optind+1], &endptr, 10);
        }
        else if (optind == 3) {
            port = (size_t) strtoull(argv[1], &endptr, 10);
        }
        else {
            fprintf (stderr, "Too many arguments\n");
            fprintf(stderr, "usage: [-t num_threads] <port>\n");
            return EXIT_FAILURE;
        }
        debug("arg:%s, %d\n", argv[optind], optind);
    }
    debug("ind:%d\n", optind);
    // char *endptr = NULL;
    // size_t port = (size_t) strtoull(argv[1], &endptr, 10);
    // size_t port = -1;
    // if (optind == 2) {
    //     port = (size_t) strtoull(argv[optind], &endptr, 10);
    // }
    // else if (optind == 3) {
    //     port = (size_t) strtoull(argv[optind], &endptr, 10);
    // }
    // else {
    //     fprintf (stderr, "Too many arguments\n");
    //     fprintf(stderr, "usage: -t <num_threads> <port>\n");
    //     return EXIT_FAILURE;
    // }
    if (num_threads == 0) {
        num_threads = 4;
        port = (size_t) strtoull(argv[1], &endptr, 10);
    }
    debug("port:%zu\n", port);
    
    if (endptr && *endptr != '\0') {
        warnx("invalid port number: %s", argv[1]);
        return EXIT_FAILURE;
    }

    //int num_threads = atoi(argv[2]);
    if (num_threads < 1 || num_threads > 200) {
        warnx("invalid thread amount: %d", num_threads);
        return EXIT_FAILURE;
    }

    signal(SIGPIPE, SIG_IGN);
    Listener_Socket sock;
    listener_init(&sock, port);
    
    // queue_t *bounded_buf = NULL;
    bounded_buf = queue_new(10);
    // pthread_mutex_t *creation_lock;
    // pthread_cond_t *creation_waiting;

    //pthread_t dispatcher; 
    //pthread_create(&dispatcher, NULL, dispatcher, NULL);
    pthread_t *workers;
    workers = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&workers[i], NULL, worker, NULL);//(void *)bounded_buf);
    }

    while (1) {
        // int connfd = listener_accept(&sock);
        uintptr_t connfd = listener_accept(&sock);
        if (connfd > 2) {
            queue_push(bounded_buf, (void *)connfd);
        }
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(workers[i], NULL);
    }

    queue_delete(&bounded_buf);

    return EXIT_SUCCESS;
}

// void *dispatcher() { nvm, main is dispatcher

// }

void *worker() {//(void *buf) {
    //queue_t *bounded_buf = (queue_t *)buf;
    // int connfd = -1;
    uintptr_t connfd = -1;
    //fprintf(stdout, "counte: %d\n", bounded_buf->count);
    while (1) {
        queue_pop(bounded_buf, (void **)&connfd);
        debug("connfd: %lu\n", connfd);
        if (connfd > 2) {
            handle_connection(connfd);
            close(connfd);
        }
        // queue_pop(bounded_buf, (void *)&connfd);
        // debug("connfd: %d\n", connfd);
        // if (connfd > 2) {
        //     handle_connection(connfd);
        //     close(connfd);
        // }
    }
}

void audit_log(conn_t *conn, const Response_t *res, const Request_t *req) {
    const char *oper = request_get_str(req);
    char *uri = conn_get_uri(conn);
    uint16_t status_code = response_get_code(res);
    char *request_id = conn_get_header(conn, "Request-Id");
    if (request_id == NULL) {
        fprintf(stderr, "%s,%s,%d,%d\n", oper, uri, status_code, 0);
    }
    else {
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
    
    // char *uri = conn_get_uri(conn);
    // char *request_id = conn_get_header(conn, "Request-Id");
    // audit_log(req, uri, res, request_id);

    conn_delete(&conn);
}

void handle_get(conn_t *conn) {

    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;
    // debug("GET request not implemented. But, we want to get %s", uri);

    // What are the steps in here?
    if (access(uri, F_OK) == 0) {
        res = &RESPONSE_OK;
    }
    
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
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
        }
        goto out;
    }

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
    //conn_send_response(conn, res);
    
    if (res == &RESPONSE_OK) {
        flock(fd, LOCK_SH);
        conn_send_file(conn, fd, filesize);
        flock(fd, LOCK_UN);
    }
    if (fd > 0) {
        close(fd);
    }
    audit_log(conn, res, &REQUEST_GET);
}

void handle_unsupported(conn_t *conn) {
    debug("handling unsupported request");

    // send responses
    conn_send_response(conn, &RESPONSE_NOT_IMPLEMENTED);
    audit_log(conn, &RESPONSE_NOT_IMPLEMENTED, &REQUEST_UNSUPPORTED);
}

void handle_put(conn_t *conn) {//, pthread_mutex_t *creation_lock, pthread_cond_t *creation_waiting) {

    char *uri = conn_get_uri(conn);
    const Response_t *res = NULL;
    debug("handling put request for %s", uri);

    // Check if file already exists before opening it.
    // pthread_mutex_lock(&creation_lock);
    // while (creating) {
    //     pthread_cond_wait(&creation_waiting, &creation_lock);
    // }
    bool existed = access(uri, F_OK) == 0;
    debug("%s existed? %d", uri, existed);

    // Open the file..
    // if (existed != 0) {
    //     pthread_mutex_lock(creation_lock);
    //     while (turn != i) {
    //         pthread_cond_wait(creation_waiting, creation_lock);
    //     }
    //     //create file
    //     pthread_mutex_unlock(creation_lock);
    //     pthread_cond_signal(creation_waiting);
    // } 

    if (existed == 0) {
        creating = true;
        open(uri, O_CREAT | O_WRONLY, 0600);
        res = &RESPONSE_CREATED;
    }

    // int fd = open(uri, O_CREAT | O_TRUNC | O_WRONLY, 0600);
    int fd = open(uri, O_TRUNC | O_WRONLY, 0600);
    flock(fd, LOCK_EX);
    if (fd < 0) {
        debug("%s: %d", uri, errno);
        if (errno == EACCES || errno == EISDIR || errno == ENOENT) {
            res = &RESPONSE_FORBIDDEN;
            flock(fd, LOCK_UN);
            goto out;
        } else {
            res = &RESPONSE_INTERNAL_SERVER_ERROR;
            flock(fd, LOCK_UN);
            goto out;
        }
    }
    creating = false;
    // pthread_mutex_unlock(&creation_lock);
    // pthread_cond_signal(&creation_waiting);

    res = conn_recv_file(conn, fd);
    flock(fd, LOCK_UN);

    if (res == NULL) {
        res = &RESPONSE_OK;
    }

    // if (res == NULL && existed) {
    //     res = &RESPONSE_OK;
    // } else if (res == NULL && !existed) {
    //     res = &RESPONSE_CREATED;
    // }

    close(fd);

out:
    conn_send_response(conn, res);
    audit_log(conn, res, &REQUEST_PUT);
}
