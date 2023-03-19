#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cacher.h"

set *set_create() {
    set *s = (set *) malloc(sizeof(set));
    s->size = 0;
    s->list = NULL;
    return s;
}

void set_destroy(set *s) {
    for (int i = 0; i < s->size; i++) {
        if (s->list[i] != NULL) {
            free(s->list[i]);
        }
    }
    free(s->list);
    free(s);
}

bool set_contains(set *s, char* data) {
    // printf("data is %s\n", data);
    for (int i = 0; i < s->size; i++) {
        // printf("%s\n", s->list[i]->data);
        if (s->list[i] != NULL && !strcmp(data, s->list[i])) {
            return true;
        }
    }
    return false;
}

bool set_add(set *s, char* data) {
    if (set_contains(s, data)) {
        return false;
    }

    s->size++;
    s->list = (char **) realloc(s->list, s->size * sizeof(char *));

    // char *new_item = (char *) malloc(sizeof(char));
    s->list[s->size - 1] = data;
    // printf("added %s\n", s->list[s->size - 1]->data);
    return true;
}

void print_set(set *s) {
    printf("Accessed: ");
    for (int i = 0; i < s->size; i++) {
        printf("%s | ", s->list[i]);
    }
    printf("\n");
}
