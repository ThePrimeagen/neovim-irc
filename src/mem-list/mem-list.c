#include "mem-list.h"

#include "string.h"
#include "stdio.h"

MemoryNode* memories = NULL;
int create_count = 0;

// this will cause 3 segfaults before the nights over
MemoryNode* create_memory() {
    printf("Creating memory %d\n", ++create_count);
    MemoryNode* mem = (MemoryNode*)malloc(sizeof(MemoryNode));
    mem->length = 0;
    mem->next = NULL;
    mem->prev = NULL;

    memset(mem->data, 0, MEMORY_MAX_SIZE);

    return mem;
}

MemoryNode* get_memory() {
    if (!memories) {
        return create_memory();
    }

    MemoryNode* mem = memories;
    if (memories->prev) {
        memories = memories->prev;
        memories->next->prev = NULL;
        memories->next = NULL;
    } else {
        memories = NULL;
    }

    memset(mem->data, 0, MEMORY_MAX_SIZE);
    return mem;
}

void release_memory(MemoryNode* mem) {
    // MUTEX ANYONE?
    // maybe, depends on how the callbacks work
    if (memories == NULL) {
        memories = mem;
    } else {
        memories->next = mem;
        mem->prev = memories;
        memories = mem;
    }
}
