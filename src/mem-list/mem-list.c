#include "mem-list.h"
#include "string.h"

MemoryNode* memories = NULL;
int max_size = 1000;

// this will cause 3 segfaults before the nights over
MemoryNode* create_memory() {
    MemoryNode* mem = (MemoryNode*)malloc(sizeof(MemoryNode));
    mem->length = 0;
    mem->next = NULL;
    mem->prev = NULL;
    mem->data = (char*)malloc(sizeof(char) * max_size);
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

    memset(mem->data, 0, sizeof(char*) * max_size);
    return mem;
}

void release_memory(MemoryNode* mem) {
    if (!memories) {
        memories = mem;
    } else {
        memories->next = mem;
        mem->prev = memories;
        memories = mem;
    }
}

int get_max_size() {
    return max_size;
}
