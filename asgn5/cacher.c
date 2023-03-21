#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cacher.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: ./cacher [-N size] <policy>\n");
        return EXIT_FAILURE;
    }
    int cache_size = 0;
    enum Policy p = 0;
    int arg = 0;
    opterr = 0;
    //accepts the optional arguments when the program starts
    while ((arg = getopt(argc, argv, "N:FLC")) != -1) {
        switch (arg) {
        case 'N': cache_size = atoi(optarg); break;
        case 'F': p = FIFO; break;
        case 'L': p = LRU; break;
        case 'C': p = CLOCK; break;
        case '?': fprintf(stderr, "usage: ./cacher [-N size] <policy>\n"); return EXIT_FAILURE;
        default: p = FIFO;
        }
    }
    if (cache_size < 1) {
        fprintf(stderr, "usage: ./cacher [-N size] <policy>\n");
        return EXIT_FAILURE;
    }

    cache *c = cache_create(cache_size);
    set *requestedItems = set_create();

    if (p == LRU) {
        runLRUcache(c, requestedItems);
    } else if (p == CLOCK) {
        runCLOCKcache(c, requestedItems);
    } else {
        runFIFOcache(c, requestedItems);
    }

    // print_cache(c);
    // print_set(requestedItems);
    printf("%d %d\n", c->comMiss, c->capMiss);

    cache_destroy(c);
    set_destroy(requestedItems);

    return EXIT_SUCCESS;
}
