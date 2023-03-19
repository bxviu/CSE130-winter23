#include <stdbool.h>
#include <stdlib.h>
#include "cacher.h"

set *set_create() {
    set *s = (set *) malloc(sizeof(set));
    s->size = 0;
    s->items = NULL;
    return s;
}

void set_destroy(set *s) {
    for (int i = 0; i < s->size; i++) {
        if (s->items[i] != NULL) {
            free(s->items[i]);
        }
    }
    free(s->items);
    free(s);
}

bool set_contains(set *s, void *data) {
    for (int i = 0; i < s->size; i++) {
        if (s->items[i] != NULL && s->items[i]->data == data) {
            return true;
        }
    }
    return false;
}

bool set_add(set *s, void *data) {
    if (set_contains(s, data)) {
        return false;
    }

    s->size++;
    s->items = (item **) realloc(s->items, s->size * sizeof(item *));

    item *new_item = (item *) malloc(sizeof(item));
    new_item->refBit = false;
    new_item->data = data;
    s->items[s->size - 1] = new_item;

    return true;
}

bool set_remove(set *s, void *data) {
    for (int i = 0; i < s->size; i++) {
        if (s->items[i] != NULL && s->items[i]->data == data) {
            free(s->items[i]);
            s->items[i] = NULL;
            for (int j = i + 1; j < s->size; j++) {
                s->items[j - 1] = s->items[j];
            }
            s->size--;
            s->items = (item **) realloc(s->items, s->size * sizeof(item *));
            return true;
        }
    }
    return false;
}
