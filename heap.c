/* NICOLESCU David-Stefan - 313CA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Copyright devmind lab
#define DIE(assertion, call_description)  \
    do                                    \
    {                                     \
        if (assertion)                    \
        {                                 \
            fprintf(stderr, "(%s, %d): ", \
                    __FILE__, __LINE__);  \
            perror(call_description);     \
            exit(errno);                  \
        }                                 \
    } while (0)

#include "heap.h"

// used for keeping the heap in the needed order
// first comparing relevance scores, then lexicographically
int compare_priority(file_node_t *f, file_node_t *s) {
    if (f->relevance_score != s->relevance_score) {
        return f->relevance_score > s->relevance_score;
    }

    return strcmp(f->id, s->id) < 0;
}

// custom function for swapping to file nodes inbetween them
void swapping_two_files(file_node_t **f, file_node_t **s) {
    file_node_t *temp = *f;
    *f = *s;
    *s = temp;
}

// function that creates the heap, returns created heap
heap_t *create_heap(int capacity)
{
    heap_t *new_heap = malloc(sizeof(*new_heap));
    DIE(new_heap == NULL, "malloc new heap");

    // case invalid given capacity
    if (capacity <= 0) {
        capacity = 1;
    }

    new_heap->heap_data = malloc(capacity * sizeof(*new_heap->heap_data));
    DIE(new_heap->heap_data == NULL, "malloc new heap data");

    new_heap->size = 0;
    new_heap->capacity = capacity;

    return new_heap;
}

// function that puts a new element in the heap, in the correct order
void push_heap(heap_t *heap, file_node_t *file)
{
    if (heap == NULL || file == NULL) {
        return;
    }

    int index, parent;
    file_node_t **realloc_file;

    // if we already at the maximum heap capacity, we make the heap bigger
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;

        realloc_file = realloc(heap->heap_data,
                               heap->capacity * sizeof(*heap->heap_data));
        DIE(realloc_file == NULL, "realloc maike heap bigger");

        heap->heap_data = realloc_file;
    }

    // first, we add it at the end of the heap
    index = heap->size++;
    heap->heap_data[index] = file;

    // we work our way back, till we find the correct position
    while (index > 0) {
        parent = (index - 1) / 2;
        // case it arrived at the right place in heap
        if (compare_priority(heap->heap_data[parent],
                             heap->heap_data[index])) {
            break;
        }
        swapping_two_files(&heap->heap_data[index], &heap->heap_data[parent]);
        index = parent;
    }
}

// function that returns the root of the heap, so the best node
// and then updates the entire heap
file_node_t *pop_heap(heap_t *heap)
{
    if (heap == NULL || heap->size == 0) {
        return NULL;
    }

    file_node_t *first_in_heap;
    int index = 0, left_son, right_son, good_node;

    // we store root, which we'll return at the end of the function
    first_in_heap = heap->heap_data[index];

    // we move the last heap element to the front, and decrease the heap size
    heap->heap_data[index] = heap->heap_data[--heap->size];

    // restore heap order
    while (10) {
        left_son = index * 2 + 1;
        right_son = 2 * (index + 1);
        good_node = index;

        if (left_son < heap->size &&
            compare_priority(heap->heap_data[left_son],
                             heap->heap_data[good_node])) {
            good_node = left_son;
        }
        if (right_son < heap->size &&
            compare_priority(heap->heap_data[right_son],
                             heap->heap_data[good_node])) {
            good_node = right_son;
        }

        if (good_node == index) {
            break;
        }

        swapping_two_files(&heap->heap_data[index],
                           &heap->heap_data[good_node]);
        index = good_node;
    }

    return first_in_heap;
}

// function that frees a heap
void free_heap(heap_t *heap)
{
    if (heap == NULL) {
        return;
    }

    free(heap->heap_data);
    free(heap);
}
