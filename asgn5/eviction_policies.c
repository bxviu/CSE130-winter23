#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cacher.h"

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

void print(bool hit) {
    if (hit) {
        fprintf(stdout, "HIT\n");
    } else {
        fprintf(stdout, "MISS\n");
    }
}

void runFIFOcache(cache *c, set *reqItems) {
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        // print_cache(c);
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        } else {
            buffer[strlen(buffer)] = '\0';
        }
        if ((int) strlen(buffer) == 0) {
            break;
        }
        char *item = strdup(buffer);
        bool access = insert_FIFO(c, item, reqItems);
        print(access);
        // print_cache(c);
    }
}

bool insert_FIFO(cache *c, char *data, set *reqItems) {
    if (cache_contains(c, data, false)) {
        free(data);
        return HIT;
    }
    if (set_contains(reqItems, data)) {
        c->capMiss++;
    } else {
        c->comMiss++;
    }
    if (c->count == c->size) {
        void *rem = remove_front(c);
        set_add(reqItems, rem);
    }
    cache_append(c, data);
    return MISS;
}

void runLRUcache(cache *c, set *reqItems) {
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        // print_cache(c);
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        } else {
            buffer[strlen(buffer)] = '\0';
        }
        if ((int) strlen(buffer) == 0) {
            break;
        }
        char *item = strdup(buffer);
        bool access = insert_LRU(c, item, reqItems);
        print(access);
        // print_cache(c);
    }
}

bool insert_LRU(cache *c, char *data, set *reqItems) {
    if (cache_contains(c, data, false)) {
        void *rem = remove_first(c, data);
        cache_append(c, rem);
        free(data);
        return HIT;
    }
    if (set_contains(reqItems, data)) {
        c->capMiss++;
    } else {
        c->comMiss++;
    }
    if (c->count == c->size) {
        void *rem = remove_front(c);
        set_add(reqItems, rem);
    }
    cache_append(c, data);
    return MISS;
}

void runCLOCKcache(cache *c, set *reqItems) {
    int *clockhand = malloc(sizeof(int));
    *clockhand = 0;
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        // print_cache(c);
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        } else {
            buffer[strlen(buffer)] = '\0';
        }
        if ((int) strlen(buffer) == 0) {
            break;
        }
        char *item = strdup(buffer);
        bool access = insert_CLOCK(c, item, reqItems, clockhand);
        print(access);
        // print_cache(c);
    }
    free(clockhand);
}

bool insert_CLOCK(cache *c, char *data, set *reqItems, int *clockhand) {
    if (cache_contains(c, data, true)) {
        free(data);
        return HIT;
    }
    if (set_contains(reqItems, data)) {
        c->capMiss++;
    } else {
        c->comMiss++;
    }
    while (true) {
        item *current_item = c->list[*clockhand];
        if (current_item != NULL) {
            if (current_item->refBit == 1) {
                current_item->refBit = 0;
                *clockhand = (*clockhand + 1) % c->size;
            } else {
                item *new = create_item(data);
                c->list[*clockhand] = new;
                set_add(reqItems, current_item->data);
                free(current_item);
                break;
            }
        } else {
            cache_append(c, data);
            break;
        }
    }
    *clockhand = (*clockhand + 1) % c->size;
    return MISS;
}
