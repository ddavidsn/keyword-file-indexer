/* NICOLESCU David-Stefan - 313CA */

#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

// structure for the heap
typedef struct heap_t {
    file_node_t **heap_data; // array for the data
    int size; // current heap size
    int capacity; // current maximum heap capacity
} heap_t;

// headers for all functions, will be explained each in the .c file
int compare_priority(file_node_t *f, file_node_t *s);
heap_t *create_heap(int capacity);
void push_heap(heap_t *heap, file_node_t *file);
file_node_t *pop_heap(heap_t *heap);
void free_heap(heap_t *heap);

#endif
