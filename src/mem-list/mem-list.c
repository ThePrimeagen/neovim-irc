#include "mem-list.h"
#include "string.h"

MemoryNode* memories;
int max_size = 1000;

void create_memory(MemoryNode* mem) {
    mem = (MemoryNode*)malloc(sizeof(MemoryList));
    mem->next = NULL;
    mem->prev = NULL;
    mem->data = (char*)malloc(sizeof(char*) * max_size);
}

void get_memory(MemoryNode* mem) {
    if (!memories) {
        create_memory(mem);
        return;
    }

    mem = memories;
    if (memories->prev) {
        memories->prev->next = NULL;
        memories->prev = NULL;
    } else {
        memories = NULL;
    }

    memset(mem->data, 0, sizeof(char*) * max_size);
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
