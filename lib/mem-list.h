#ifndef MEM_LIST_HEADER_H
#define MEM_LIST_HEADER_H

#include <stdlib.h>

#define MEMORY_MAX_SIZE 1024

typedef struct MemoryNode {
    struct MemoryNode* prev;
    struct MemoryNode* next;
    int length;
    char data[MEMORY_MAX_SIZE];
} MemoryNode;

MemoryNode* get_memory();
void release_memory(MemoryNode* mem);

#endif


