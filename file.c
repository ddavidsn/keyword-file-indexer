/* NICOLESCU David-Stefan - 313CA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "file.h"

// Copyright devmind lab
#define DIE(assertion, call_description)                                      \
	do {                                                                      \
		if (assertion) {                                                      \
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                \
			perror(call_description);                                         \
			exit(errno);                                                      \
		}                                                                     \
	} while (0)

#define INIT_KW_CAPACITY 8

// function for making a separate copy of string
char *custom_strdup(const char *s) {
    char *copy = malloc(strlen(s) + 1);
    DIE(copy == NULL, "malloc strdup");

    strcpy(copy, s);
    return copy;
}

// function that creates the file list
file_dllist_t *create_file_list(void)     
{
    file_dllist_t *f_list = malloc(sizeof(*f_list));
    DIE(f_list == NULL, "malloc create list for files");

    f_list->head = NULL;
    f_list->tail = NULL;
    f_list->size = 0;

    return f_list;
}

// function that creates a single file node, storing id and relevance score
file_node_t *create_file(char *id, int score)
{
    file_node_t *new_file = malloc(sizeof(*new_file));
    DIE(new_file == NULL, "malloc create new list");

    new_file->relevance_score = score;
    new_file->id = custom_strdup(id);
    new_file->kw_capacity = INIT_KW_CAPACITY;
    new_file->kw_count = 0;
    new_file->prev = NULL;
    new_file->next = NULL;

    new_file->keywords = calloc(new_file->kw_capacity,
                                sizeof(*new_file->keywords));
    DIE(new_file->keywords == NULL, "calloc keywords of file");

    return new_file;
}

// function that adds a single keyword to a file node
void add_keyword_to_file(file_node_t *file, char *keyword)
{
    // first, safety measures and verification to avoid duplicates
    if (file == NULL || file_has_kw(file, keyword) == 1) {
        return;
    }

    // if necessary, make the keywords array bigger
    char **realloc_kw;

    if (file->kw_count == file->kw_capacity) {
        file->kw_capacity *= 2;
        realloc_kw = realloc(file->keywords,
                             file->kw_capacity* sizeof(*file->keywords));
        DIE(realloc_kw == NULL, "realloc extension keywords");
        
        file->keywords = realloc_kw;
    }

    // add keyword on last pos in keywords array and increase the current len
    file->keywords[file->kw_count++] = custom_strdup(keyword);
}

// bool function that tells us if a file has or doesn't have a keyword
int file_has_kw(file_node_t *file, char *keyword)
{
    if (file == NULL) {
        return 0;
    }

    // iteration through all keywords
    for (int i = 0; i < file->kw_count; i++) {
        if (strcmp(file->keywords[i], keyword) == 0) {
            return 1;
        }
    }

    return 0;
}

// function that deletes a specific keyword from a given file
void remove_kw_from_file(file_node_t *file, char *keyword)
{
    // safety + check if it doesn't have keyword
    if (file == NULL || file_has_kw(file, keyword) == 0) {
        return;
    }

    // first, we find the position of the keyword
    int pos_of_kw = -1;

    for (int i = 0; i < file->kw_count; i++) {
        if (strcmp(file->keywords[i], keyword) == 0) {
            pos_of_kw = i;
            break;
        }
    }

    free(file->keywords[pos_of_kw]);

    // we move the positions of the rest of keywords
    for (int i = pos_of_kw; i < file->kw_count - 1; i++) {
        file->keywords[i] = file->keywords[i + 1];
    }

    // decrease current len
    file->kw_count--;
    // make the last element, that's useless now, null
    file->keywords[file->kw_count] = NULL;
}

// function that returns the file node with the specified id of a files list
// or NULL, if the list doesn't contain the file
file_node_t *find_file(file_dllist_t *list, char *id)
{
    if (list == NULL || list->head == NULL) {
        return NULL;
    }

    file_node_t *curr = list->head;

    while (curr != NULL) {
        if (strcmp(curr->id, id) == 0) {
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}

// function that adds a file node to a list
// easily done with the help of the tail
void add_file_to_list(file_dllist_t *list, file_node_t *file)
{
    if (list == NULL || file == NULL) {
        return;
    }

    // case first file node of the list
    if (list->head == NULL && list->tail == NULL) {
        list->head = list->tail = file;
        list->size++;
        return;
    }

    list->tail->next = file;
    file->prev = list->tail;
    list->tail = file;
    list->size++;
    return;
}

// function that removes a file node from a files list
void remove_file_from_list(file_dllist_t *list, file_node_t *file)
{
    if (list == NULL || file == NULL) {
        return;
    }

    // case: list doesn't have specified file
    if (find_file(list, file->id) == NULL) {
        return;
    }

    // check if given file is head
    if (file->prev != NULL) {
        file->prev->next = file->next;
    } else {
        list->head = file->next;
    }

    // check if given file is tail
    if (file->next != NULL) {
        file->next->prev = file->prev;
    } else {
        list->tail = file->prev;
    }

    file->next = NULL;
    file->prev = NULL;
    list->size--;
}

// function used for freeing a file node
void free_file(file_node_t *file)
{
    if (file == NULL) {
        return;
    }

    for (int i = 0; i < file->kw_count; i++) {
        free(file->keywords[i]);
    }

    free(file->keywords);
    free(file->id);
    free(file);
}

// function used for freeing an entire files list
void free_list_of_files(file_dllist_t *list)
{
    if (list == NULL) {
        return;
    }

    file_node_t *curr, *next_curr;

    curr = list->head;

    while(curr != NULL) {
        next_curr = curr->next;
        free_file(curr);
        curr = next_curr;
    }

    free(list);
}
