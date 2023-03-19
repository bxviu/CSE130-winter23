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
    }
    else {
        fprintf(stdout, "MISS\n");
    }
}

void runFIFOcache(cache* c, set* reqItems) {
    // void* item = (void*)1;
    while(true) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer)-1] = '\0';
        printf("got: |%s|\n", buffer);
        char* item = strdup(buffer);
        bool access = insert_FIFO(c, item, reqItems);
        print(access);
    }
}

bool insert_FIFO(cache* c, void* item, set* reqItems) {
    if (contains(c, item, false)) {
        return HIT;
    }
    if (set_contains(reqItems, item)) {
        c->capMiss++;
    }
    else {
        c->comMiss++;
    }
    if (c->count == c->size) {
        void* rem = remove_front(c);
        set_add(reqItems, rem);
    }
    append(c, item);
    return MISS;
}

void runLRUcache(cache* c, set* reqItems) {
    void* item = (void*)1;
    while(true) {
        bool access = insert_LRU(c, item, reqItems);
        print(access);
    }
}

bool insert_LRU(cache* c, void* item, set* reqItems) {
    if (contains(c, item, false)) {
        void* rem = remove_first(c, item);
        append(c, rem);
        return HIT;
    }
    if (set_contains(reqItems, item)) {
        c->capMiss++;
    }
    else {
        c->comMiss++;
    }
    if (c->count == c->size) {
        void* rem = remove_front(c);
        set_add(reqItems, rem);
    }
    append(c, item);
    return MISS;
}

void runCLOCKcache(cache* c, set* reqItems) {
    int clockhand = 0;
    void* item = (void*)1;
    while(true) {
        bool access = insert_CLOCK(c, item, reqItems, clockhand);
        print(access);
    }
}

bool insert_CLOCK(cache* c, void* data, set* reqItems, int clockhand) {
    if (contains(c, data, true)) {
        return HIT;
    }
    if (set_contains(reqItems, data)) {
        c->capMiss++;
    }
    else {
        c->comMiss++;
    }
    while (true) {
        item* current_item = c->list[clockhand]; 
        if (current_item != NULL) {
            if (current_item->refBit == 1) {
                current_item->refBit = 0;
                clockhand++;
            }
            else {
                void* rem = remove_first(c, current_item->data);
                set_add(reqItems, rem);
                break;
            }
        }
        else {
            break;
        }
    }
    append(c, data);
    clockhand++;
    return MISS;
}
