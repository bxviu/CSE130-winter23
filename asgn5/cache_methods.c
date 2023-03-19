#include <stdbool.h>
#include <stdlib.h>
#include "cacher.h"

cache *create(int size) {
    cache *c = malloc(sizeof(cache));
    c->size = size;
    c->list = malloc(c->size * sizeof(item *));
    c->comMiss = 0;
    c->capMiss = 0;
    c->count = 0;
    return c;
}

bool contains(cache* c, void* data, bool setBit) {
    for (int i = 0; i < c->count; i++) {
        if (data == c->list[i]->data) {
            if (setBit) {
                c->list[i]->refBit = 1;
            }
            return true;
        }
    }
    return false;
}

void* remove_front(cache* c) {
    if (c->count == 0) {
        return NULL;
    }
    item* front_item = c->list[0];
    void* front_data = front_item->data;
    for (int i = 0; i < c->count - 1; i++) {
        c->list[i] = c->list[i + 1];
    }
    c->count--;
    free(front_item);
    return front_data;
}

void* remove_first(cache* c, void* data) {
    for (int i = 0; i < c->count; i++) {
        if (c->list[i]->data == data) {
            item* removed_item = c->list[i];
            void* removed_data = removed_item->data;
            for (int j = i; j < c->count - 1; j++) {
                c->list[j] = c->list[j+1];
            }
            c->count--;
            free(removed_item);
            return removed_data;
        }
    }
    return NULL;
    // void* remove = NULL;
    // for (int i = 0; i < c->count; i++) {
    //     if (*(item) == *(c->list[i]->data)) {
    //         remove = c->list[i]->data;
    //         free(c->list[i]);
    //         for (int j = i; j < c->count - 1; j++) {
    //             c->list[j] = c->list[j+1];
    //         }
    //         c->count--;
    //         break;
    //     }
    // }
    // return remove;
}

bool append(cache* c, void* data) {
    if (c->count == c->size) {
        return false;
    }
    item* new_item = (item*) malloc(sizeof(item));
    new_item->refBit = false;
    new_item->data = data;
    c->list[c->count] = new_item;
    c->count++;
    return true;
}

void destroy(cache *c) {
    for (int i = 0; i < c->size; i++) {
        if (c->list[i] != NULL) {
            free(c->list[i]);
        }
    }
    free(c->list);
    free(c);
}
