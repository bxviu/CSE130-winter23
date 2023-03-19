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
        warnx("wrong arguments: %s port_num", argv[0]);
        fprintf(stderr, "usage: ./cacher [-N size] <policy>\n");
        return EXIT_FAILURE;
    }
    int cache_size = -1;
    enum Policy p = 0;
    int arg = 0;
    opterr = 0;
    //accepts the optional arguments when the program starts
    while ((arg = getopt(argc, argv, "N:FLC")) != -1) {
        switch (arg) {
        case 'N':
            cache_size = atoi(argv[optind]);
            break;
        case 'F':
            p = FIFO;
            break;
        case 'L':
            p = LRU;
            break;
        case 'C':
            p = CLOCK;
            break;
        case '?':
            fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            fprintf(stderr, "usage: ./cacher [-N size] <policy>\n");
            return EXIT_FAILURE;
        default:
            p = FIFO;
        }
    }
    if (cache_size == -1) {
        warnx("wrong arguments: %s port_num", argv[0]);
        fprintf(stderr, "usage: ./cacher [-N size] <policy>\n");
        return EXIT_FAILURE;
    }

    cache *c = create(cache_size);
    set *requestedItems = set_create();
    
    if (p == LRU) {
        printf("lru");
        //runLRUcache(c, requestedItems);
    }
    else if (p == CLOCK) {
        printf("clock");
        //runCLOCKcache(c, requestedItems);
    } 
    else {
        printf("fifo");
        runFIFOcache(c, requestedItems);
    }

    // read_input();

    set_destroy(requestedItems);
    destroy(c);

    return EXIT_SUCCESS;
}

