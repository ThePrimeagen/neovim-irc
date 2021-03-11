#ifndef MEM_LIST_HEADER_H
#define MEM_LIST_HEADER_H

#include <stdlib.h>

typedef struct MemoryNode {
    struct MemoryNode* prev;
    struct MemoryNode* next;
    char* data;
    int length;
} MemoryNode;

MemoryNode* get_memory();
void release_memory(MemoryNode* mem);
int get_max_size();

#endif


