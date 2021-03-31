#include "mem-list.h"

#include "string.h"
#include "stdio.h"

MemoryNode* memories = NULL;
int create_count = 0;

// this will cause 3 segfaults before the nights over
MemoryNode* create_memory() {
    MemoryNode* mem = (MemoryNode*)malloc(sizeof(MemoryNode));
    mem->length = 0;
    mem->prev = NULL;

    memset(mem->data, 0, MEMORY_MAX_SIZE);

    return mem;
}

MemoryNode* get_memory() {
    if (!memories) {
        return create_memory();
    }

    MemoryNode* mem = memories;
    memories = mem->prev;

    memset(mem->data, 0, MEMORY_MAX_SIZE);
    return mem;
}

void release_memory(MemoryNode* mem) {
    mem->prev = memories;
    memories = mem;
}
