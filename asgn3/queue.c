#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"

struct queue {
    int in, out, count, size;
    pthread_mutex_t *lock;
    pthread_cond_t *full;
    pthread_cond_t *empty;
    void **buffer;
};

queue_t *queue_new(int size) {
    queue_t *q = malloc(sizeof(queue_t));
    q->size = size;
    q->in = 0;
    q->out = 0;
    q->count = 0;
    q->lock = malloc(sizeof(pthread_mutex_t));
    q->full = malloc(sizeof(pthread_cond_t));
    q->empty = malloc(sizeof(pthread_cond_t));
    pthread_mutex_init(q->lock, NULL);
    pthread_cond_init(q->full, NULL);
    pthread_cond_init(q->empty, NULL);
    q->buffer = malloc(size * sizeof(void *));
    return q;
}

void queue_delete(queue_t **q) {
    if (q == NULL || *q == NULL) {
        return;
    }
    pthread_mutex_destroy((*q)->lock);
    pthread_cond_destroy((*q)->full);
    pthread_cond_destroy((*q)->empty);
    free((*q)->lock);
    free((*q)->full);
    free((*q)->empty);
    free((*q)->buffer);
    free(*q);
    *q = NULL;
}

bool queue_push(queue_t *q, void *elem) {
    if (q == NULL) {
        return false;
    }
    pthread_mutex_lock(q->lock);
    while (q->count == q->size) {
        pthread_cond_wait(q->full, q->lock);
    }
    q->buffer[q->in] = elem;
    q->in = (q->in + 1) % q->size;
    q->count += 1;
    pthread_mutex_unlock(q->lock);
    pthread_cond_signal(q->empty);
    return true;
}

bool queue_pop(queue_t *q, void **elem) {
    if (q == NULL) {
        return false;
    }
    pthread_mutex_lock(q->lock);
    while (q->count == 0) {
        pthread_cond_wait(q->empty, q->lock);
    }
    *elem = q->buffer[q->out];
    q->out = (q->out + 1) % q->size;
    q->count -= 1;
    pthread_mutex_unlock(q->lock);
    pthread_cond_signal(q->full);
    return true;
}
