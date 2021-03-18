#ifndef MESSAGE_QUEUE_HEADER_H
#define MESSAGE_QUEUE_HEADER_H

#include <stdint.h>

#include "mem-list.h"
#include "user.h"

typedef struct MessageQueue {
    pthread_mutex_t lock;
} MessageQueue;

#endif

