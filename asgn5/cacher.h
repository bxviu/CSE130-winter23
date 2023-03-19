#pragma once

#include <stdbool.h>
#include <stdlib.h>

#ifndef CACHER_H
#define CACHER_H

static const bool HIT = true;
static const bool MISS = false;

#endif

enum Policy {
    FIFO = 0,
    LRU = 1,
    CLOCK = 2
};

typedef struct {
    bool refBit;
    void *data;
} item;

typedef struct {
    int count, size, comMiss, capMiss;
    item** list;
} cache;

typedef struct {
    int size;
    item** items;
} set;

cache *create(int size);
void destroy(cache* c);
bool contains(cache* c, void* data, bool setBit);
void* remove_front(cache* c);
void* remove_first(cache* c, void* data);
bool append(cache* c, void* data);

set *set_create();
void set_destroy(set *s);
bool set_contains(set *s, void *data);
bool set_add(set *s, void *data);
bool set_remove(set *s, void *data);

// void read_input();
void print(bool hit);

void runFIFOcache(cache* c, set* reqItems);
bool insert_FIFO(cache* c, void* item, set* reqItems);

void runLRUcache(cache* c, set* reqItems);
bool insert_LRU(cache* c, void* item, set* reqItems);

void runCLOCKcache(cache* c, set* reqItems);
bool insert_CLOCK(cache* c, void* item, set* reqItems, int clockhand);

