#ifndef MEM_LIST_HEADER_H
#define MEM_LIST_HEADER_H

#include <stdlib.h>

typedef struct MemoryNode {
    struct MemoryNode* prev;
    struct MemoryNode* next;
    char* data;
} MemoryNode;

void get_memory(MemoryNode* mem);
void release_memory(MemoryNode* mem);
int get_max_size();

#endif


