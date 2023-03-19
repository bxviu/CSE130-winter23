#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cacher.h"

void print_cache(cache *c) {
    printf("Cache: ");
    for (int i = 0; i < c->count; i++) {
        printf("%s:%d | ", c->list[i]->data, c->list[i]->refBit);
    }
    printf("\n");
}

item *create_item(char *data) {
    item *new_item = (item *) malloc(sizeof(item));
    new_item->refBit = false;
    new_item->data = data;
    return new_item;
}

cache *cache_create(int size) {
    cache *c = malloc(sizeof(cache));
    c->size = size;
    c->list = (item **) malloc(c->size * sizeof(item *));
    for (int i = 0; i < c->size; i++) {
        c->list[i] = NULL;
    }
    c->comMiss = 0;
    c->capMiss = 0;
    c->count = 0;
    return c;
}

void cache_destroy(cache *c) {
    for (int i = 0; i < c->size; i++) {
        if (c->list[i] != NULL) {
            if (c->list[i]->data != NULL) {
                free(c->list[i]->data);
            }
            free(c->list[i]);
        }
    }
    free(c->list);
    free(c);
}

bool cache_contains(cache *c, char *data, bool setBit) {
    for (int i = 0; i < c->count; i++) {
        if (!strcmp(data, c->list[i]->data)) {
            if (setBit) {
                c->list[i]->refBit = 1;
            }
            return true;
        }
    }
    return false;
}

bool cache_append(cache *c, char *data) {
    if (c->count == c->size) {
        return false;
    }
    item *new_item = create_item(data);
    c->list[c->count] = new_item;
    c->count++;
    return true;
}

char *remove_front(cache *c) {
    if (c->count == 0) {
        return NULL;
    }
    item *front_item = c->list[0];
    void *front_data = front_item->data;
    for (int i = 0; i < c->count - 1; i++) {
        c->list[i] = c->list[i + 1];
    }
    c->count--;
    free(front_item);
    return front_data;
}

char *remove_first(cache *c, char *data) {
    for (int i = 0; i < c->count; i++) {
        if (!strcmp(data, c->list[i]->data)) {
            item *removed_item = c->list[i];
            void *removed_data = removed_item->data;
            for (int j = i; j < c->count - 1; j++) {
                c->list[j] = c->list[j + 1];
            }
            c->count--;
            free(removed_item);
            return removed_data;
        }
    }
    return NULL;
}
