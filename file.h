/* NICOLESCU David-Stefan - 313CA */

#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structure for a node type file
typedef struct file_node_t {
    char *id; // name of the file, unique ID
    int relevance_score; // relevance score, essential for the heap sorting
    char **keywords; // list of keywords
    int kw_count; // number of keywords
    int kw_capacity; // capacity of keywords array

    struct file_node_t *prev; // doubly linked list, we store both previous
    struct file_node_t *next; // and next node, as in PDF
} file_node_t;

// structure for list of files
typedef struct file_dllist_t {
    file_node_t *head; // first file node
    file_node_t *tail; // last file node
    int size; // size of list
} file_dllist_t;

// headers of all used fucntions, each will be expalined in .c file
char *custom_strdup(const char *s);

file_dllist_t *create_file_list(void);
file_node_t *create_file(char *id, int score);

void add_keyword_to_file(file_node_t *file, char *keyword);
int file_has_kw(file_node_t *file, char *keyword);
void remove_kw_from_file(file_node_t *file, char *keyword);

file_node_t *find_file(file_dllist_t *list, char *id);

void add_file_to_list(file_dllist_t *list, file_node_t *file);
void remove_file_from_list(file_dllist_t *list, file_node_t *file);

void free_file(file_node_t *file);
void free_list_of_files(file_dllist_t *list);

#endif
