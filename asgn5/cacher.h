#pragma once

#include <stdbool.h>
#include <stdlib.h>

#ifndef CACHER_H
#define CACHER_H

static const bool HIT = true;
static const bool MISS = false;

#endif

enum Policy { FIFO = 0, LRU = 1, CLOCK = 2 };

typedef struct {
    bool refBit;
    char *data;
} item;

typedef struct {
    int count, size, comMiss, capMiss;
    item **list;
} cache;

typedef struct {
    int size;
    char **list;
} set;

item *create_item(char *data);

cache *cache_create(int size);
void cache_destroy(cache *c);
bool cache_contains(cache *c, char *data, bool setBit);
char *remove_front(cache *c);
char *remove_first(cache *c, char *data);
bool cache_append(cache *c, char *data);

set *set_create();
void set_destroy(set *s);
bool set_contains(set *s, char *data);
bool set_add(set *s, char *data);

void print(bool hit);
void print_cache(cache *c);
void print_set(set *s);

void runFIFOcache(cache *c, set *reqItems);
bool insert_FIFO(cache *c, char *data, set *reqItems);

void runLRUcache(cache *c, set *reqItems);
bool insert_LRU(cache *c, char *data, set *reqItems);

void runCLOCKcache(cache *c, set *reqItems);
bool insert_CLOCK(cache *c, char *data, set *reqItems, int *clockhand);
