/* NICOLESCU David-Stefan - 313CA */

#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"

// structure for the nodes that indicate the file each keyword appears in
// found at the end of keywords, starting from the terminal node of word
typedef struct ref_trie_node_t {
    file_node_t *file;
    struct ref_trie_node_t *next;
} ref_trie_node_t;

// structure of a trie node, typical, like taught in the lab
typedef struct trie_node_t {
    int nr_children;
    struct trie_node_t** children;
    int end_of_word;
    ref_trie_node_t *refs;
} trie_node_t;

// structure of the trie
typedef struct trie_t {
    trie_node_t* root;

    int alphabet_size;
    int size;
    int nNodes;
} trie_t;

// headers of all used functions, will be explained in the .c file
trie_node_t *create_trie_node(trie_t *trie);
trie_t* trie_create(void);

trie_node_t *insert_word_trie(trie_t *trie, trie_node_t *curr, char *word,
                              file_node_t *file);
trie_node_t *find_word_trie(trie_t *trie, char *word);
trie_node_t *find_prefix_node(trie_t *trie, char *prefix);

int node_has_ref(trie_node_t *tr_node, file_node_t *file);
void add_ref_trie(trie_node_t *node_tr, file_node_t *file);
int remove_ref_trie(trie_node_t *tr_node, file_node_t *file);
int count_ref_trie(trie_node_t *node_tr);
void free_refs(ref_trie_node_t *ref);

int useless_trie_node(trie_node_t *node);
int delete_word_helper(trie_t *trie, trie_node_t *curr, char *word);
void del_word_if_empty_trie(trie_t *trie, char *word);

void print_refs(trie_node_t *tr_node, FILE *fout);
void print_empty_message(FILE *fout);
void helper_print_trie(trie_node_t *tr_node, char *word, int dep, FILE *fout);
void print_trie(trie_t *trie, FILE *fout);
void print_for_find_command(trie_t *trie, char *word, FILE *fout);

int arr_alr_has_file(file_node_t **arr, int *cnt, file_node_t *file);
void adding_uniq_file(file_node_t ***f_arr, int *cnt, int *capac,
                      file_node_t *file);
void find_prefix_files(trie_node_t *tr_node, file_node_t ***f_arr, int *cnt,
                       int *capac);

void free_node_trie(trie_node_t *tr_node);
void free_trie(trie_t *trie);

#endif
