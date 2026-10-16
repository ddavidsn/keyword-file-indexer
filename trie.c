/* NICOLESCU David-Stefan - 313CA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "trie.h"

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

#define ALPHABET_SIZE 26
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"

#define MAX_LEN 501

// function that creates a trie node, allocating dynamic memory
// and returns the node
trie_node_t* create_trie_node(trie_t * trie) {
    trie_node_t *new_node =  malloc(sizeof(*new_node));
    DIE(new_node == NULL, "malloc create new node");

    new_node->end_of_word = 0;
    new_node->nr_children = 0;

    new_node->children = calloc(trie->alphabet_size,
                                sizeof(*new_node->children));
    DIE(new_node->children == NULL, "calloc children new node");

    new_node->refs = NULL;

	return new_node;
}

// function that creates the trie and returns it
trie_t* trie_create(void) {
    trie_t *new_tree = malloc(sizeof(*new_tree));
    DIE(new_tree == NULL, "malloc create tree");

    new_tree->alphabet_size = ALPHABET_SIZE;
    new_tree->size = 0;
    new_tree->nNodes = 0;

    new_tree->root = create_trie_node(new_tree);
    new_tree->nNodes = 1;

    return new_tree;
}

// function that inserts a keyword in the trie
trie_node_t *insert_word_trie(trie_t *trie, trie_node_t *curr,
                              char *word, file_node_t *file) {
    // safety measures
    if (trie == NULL || curr == NULL || word == NULL || file == NULL) {
        return NULL;
    }

    // if we arrived at the end of the keyword to insert
    if (*word == '\0') {
        if (curr->end_of_word == 0) {
            trie->size++;
        }

        // mark end of a keyword
        curr->end_of_word = 1;
        // now we add the ref node that points to the file where the kw
        // can be found
        add_ref_trie(curr, file);
        return curr;
    }

    // index to know to which child to go
    int index = *word - 'a';

    // check against unvalid index, that isn't in the correct range
    if (index < 0 || index >= ALPHABET_SIZE) {
        return NULL;
    }

    // if the needed child doesn't exist yet, we create it
    if (curr->children[index] == NULL) {
        curr->children[index] = create_trie_node(trie);
        curr->nr_children++;
        trie->nNodes++;
    }

    // recursive function
    return insert_word_trie(trie, curr->children[index], word + 1, file);
}

// function that returns the terminal node of a keyword, if the trie
// contains the keyword, else NULL
trie_node_t *find_word_trie(trie_t *trie, char *word)
{
    // safety precautions
    if (trie == NULL || word == NULL) {
        return NULL;
    }

    trie_node_t *curr = trie->root;

    for (int i = 0; word[i]; i++) {
        int index = word[i] - 'a';
        // invalid index or the child doesn't even exist
        if (index < 0 || index >= ALPHABET_SIZE ||
            curr->children[index] == NULL) {
            return NULL;
        }
        
        curr = curr->children[index];
    }

    // also check if the given word is only a prefix, not an independent word
    if (curr->end_of_word == 0) {
        return NULL;
    }

    // returns last node
    return curr;
}

// function for the bonus task, finds and returns, if it exists
// the terminal node of a prefix sequence from the trie
// similary to the previous function, without the last check
trie_node_t *find_prefix_node(trie_t *trie, char *prefix)
{
    if (trie == NULL || prefix == NULL) {
        return NULL;
    }

    trie_node_t *curr = trie->root;
    int index;

    for (int i = 0; prefix[i] != '\0'; i++) {
        index = prefix[i] - 'a';
        if (index < 0 || index >= ALPHABET_SIZE ||
            curr->children[index] == NULL) {
            return NULL;
        }

        curr = curr->children[index];
    }

    return curr;
}

// boolean function, checks if a keyword is mentioned or not in the given file
int node_has_ref(trie_node_t *tr_node, file_node_t *file) {
    if (tr_node == NULL || file == NULL) {
        return 0;
    }

    ref_trie_node_t *curr = tr_node->refs;

    // iterations through all refs nodes
    while (curr != NULL) {
        if (curr->file == file) {
            return 1;
        }

        curr = curr->next;
    }
    return 0;
}

// function that adds a ref node to the terminal node from trie
void add_ref_trie(trie_node_t *node_tr, file_node_t *file) {
    // always safety measures
    if (node_tr == NULL || file == NULL) {
        return;
    }

    // if same keyword appears more than 1 time for same file
    // considered only one time
    if (node_has_ref(node_tr, file) == 1) {
        return;
    }

    ref_trie_node_t *ref_node = malloc(sizeof(*ref_node));
    DIE(ref_node == NULL, "malloc ref node trie");

    // we will add all refs nodes in lexicographical order, to print
    // them easier
    ref_node->file = file;
    ref_node->next = NULL;

    // case: have to add this ref node first in line
    if (node_tr->refs == NULL ||
        strcmp(file->id, node_tr->refs->file->id) < 0) {
        ref_node->next = node_tr->refs;
        node_tr->refs = ref_node;
        return;
    }

    ref_trie_node_t *curr = node_tr->refs;

    // other case: find the lexicographical position in the list
    while (curr->next != NULL && strcmp(curr->next->file->id, file->id) < 0) {
        curr = curr->next;
    }

    ref_node->next = curr->next;
    curr->next = ref_node;
}

// function that counts the number of files a keyword appears in
// input: terminal node of keyword
int count_ref_trie(trie_node_t *node_tr)
{
    int nr_of_refs = 0;

    if (node_tr == NULL) {
        return 0;
    }

    ref_trie_node_t *aux_node = node_tr->refs;

    while (aux_node != NULL) {
        nr_of_refs++;
        aux_node = aux_node->next;
    }

    return nr_of_refs;
}

// function that removes a ref node for a terminal trie node
// int function, returns 1 if successfull, otherwise 0
int remove_ref_trie(trie_node_t *tr_node, file_node_t *file)
{
    if (file == NULL || tr_node == NULL) {
        return 0;
    }

    // two nodes are needed
    ref_trie_node_t *prev_curr = NULL, *curr;
    curr = tr_node->refs;

    while (curr != NULL) {
        if (curr->file == file) {
            // case: the first in line is to be removed
            if (prev_curr == NULL) {
                tr_node->refs = curr->next;
            } else {
                prev_curr->next = curr->next;
            }

            free(curr);
            return 1;
        }

        prev_curr = curr;
        curr = curr->next;
    }

    return 0;
}

// function that frees the memory of all ref nodes for a trie node
void free_refs(ref_trie_node_t *ref) {
    if (ref == NULL) {
        return;
    }

    ref_trie_node_t *aux_ref;

    while (ref != NULL) {
        aux_ref = ref->next;
        free(ref);
        ref = aux_ref;
    }
}

// function that checks if a trei node is useless or not
// with other words, a trie node is useless if
// it's not a terminal nnode for a keyword and
// it doesn't have any children
int useless_trie_node(trie_node_t *node) {
    return node->end_of_word == 0 && node->nr_children == 0;
}

// recursive function that actually deletes nodes from trie
int delete_word_helper(trie_t *trie, trie_node_t *curr, char *word) {
    if (curr == NULL || trie == NULL || word == NULL) {
        return 0;
    }

    if (*word == '\0') {
        if (curr->end_of_word == 0 || curr->refs != NULL) {
            return 0;
        }

        // the terminal node is't terminal anymore
        curr->end_of_word = 0;
        trie->size--;

        // check if it has or not meaning for the trie anymore
        return useless_trie_node(curr);
    }

    int index = *word - 'a';

    // case invalid index, out of bounds
    if (index < 0 || index >= ALPHABET_SIZE) {
        return 0;
    }

    trie_node_t *the_child = curr->children[index];

    if (the_child == NULL) {
        return 0;
    }

    // recursive call, to know if we should delete the child
    int should_delete_child = delete_word_helper(trie, the_child, word + 1);

    // we have to delete the child
    if (should_delete_child == 1) {
        free_node_trie(the_child);
        curr->children[index] = NULL;
        curr->nr_children--;
        trie->nNodes--;
    }

    return useless_trie_node(curr);
}

// big delete function that deletes a word and checks if it should change trie
void del_word_if_empty_trie(trie_t *trie, char *word)
{
    if (trie == NULL || word == NULL) {
        return;
    }

    // keyword isn't contained in trie
    if (find_word_trie(trie, word) == NULL) {
        return;
    }

    // terminal node of keyword
    trie_node_t *tr_node = find_word_trie(trie, word);

    // if it still has ref nodes, no delete
    if (tr_node->refs != NULL) {
        return;
    }

    delete_word_helper(trie, trie->root, word);
}

// function that prints the message EMPTY for specified edge cases
void print_empty_message(FILE *fout) {
    fprintf(fout, "EMPTY\n");
}

// helper function to print all ref nodes of a trie node
void print_refs(trie_node_t *tr_node, FILE *fout) {
    ref_trie_node_t *curr_ref = tr_node->refs;

    while (curr_ref != NULL) {
        fprintf(fout, " %s", curr_ref->file->id);
        curr_ref = curr_ref->next;
    }
}

// helper function for printing whole trie
void helper_print_trie(trie_node_t *tr_node, char *word, int dep, FILE *fout) {
    if (tr_node == NULL) {
        return;
    }

    // if we arrived at the end of a keyword
    if (tr_node->end_of_word == 1) {
        word[dep] = '\0';

        int ref_cnt = count_ref_trie(tr_node);

        // also print all ref nodes, so the number of files and each file
        if (ref_cnt > 0) {
            fprintf(fout, "%s %d", word, ref_cnt);
            print_refs(tr_node, fout);
            fprintf(fout, "\n");
        }
    }

    // recursive call for each child
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (tr_node->children[i] != NULL) {
            word[dep] = 'a' + i;
            helper_print_trie(tr_node->children[i], word, dep + 1, fout);
        }
    }
}

// functions for printing the entire trie
void print_trie(trie_t *trie, FILE *fout)
{
    // case: empty trie
    if (trie == NULL || trie->root == NULL || trie->size == 0) {
        print_empty_message(fout);
        return;
    }

    // array for storing a word
    char word[MAX_LEN];

    helper_print_trie(trie->root, word, 0, fout);
}

// function used for find command, prints all files that have a given
// keyword, so all ref nodes of a terminal node from trie
void print_for_find_command(trie_t *trie, char *word, FILE *fout) {
    if (trie == NULL || word == NULL) {
        return;
    }

    // we find the terminal node of the keyword
    trie_node_t *tr_node = find_word_trie(trie, word);

    // case keyword isn't in trie or isn't mentioned in any file
    if (tr_node == NULL || tr_node->refs == NULL) {
        print_empty_message(fout);
        return;
    }

    fprintf(fout, "%d", count_ref_trie(tr_node));
    print_refs(tr_node, fout);
    fprintf(fout, "\n");
}

// function used for checking if a file id appears uniquely in the files array
int arr_alr_has_file(file_node_t **arr, int *cnt, file_node_t *file) {
    for (int i = 0; i < *cnt; i++) {
        if (arr[i] == file) {
            return 1;
        }
    }

    return 0;
}

// function that adds a file to the array, only after checking if
// the file isn't already in the array
void adding_uniq_file(file_node_t ***f_arr, int *cnt, int *capac,
                      file_node_t *file) {
    file_node_t **realloc_arr;
    int pose;

    // check against duplicates
    if (arr_alr_has_file(*f_arr, cnt, file)) {
        return;
    }

    // if necessary, we double the capacity of the array
    if (*cnt == *capac) {
        *capac *= 2;
        realloc_arr = realloc(*f_arr, *capac * sizeof(**f_arr));
        DIE(realloc_arr == NULL, "realloc array files prefix");

        *f_arr = realloc_arr;
    }

    // also, we want to add the files in lexicographical order, so that
    // the printing will be easily done
    pose = *cnt;

    while (pose > 0 && strcmp((*f_arr)[pose - 1]->id, file->id) > 0) {
        (*f_arr)[pose] = (*f_arr)[pose - 1];
        pose--;
    }

    (*f_arr)[pose] = file;
    (*cnt)++;
}

// function that creates the array containing all files that have keywords
// starting with the specified prefix
void find_prefix_files(trie_node_t *tr_node, file_node_t ***f_arr, int *cnt,
                       int *capac)
{
    if (tr_node == NULL) {
        return;
    }

    // if we arrived at the end of a word with the prefix
    if (tr_node->end_of_word == 1) {
        ref_trie_node_t *ref_node = tr_node->refs;

        // for all ref nodes, we use function to add only one single time
        // to the created array
        while (ref_node != NULL) {
            adding_uniq_file(f_arr, cnt, capac, ref_node->file);
            ref_node = ref_node->next;
        }
    }

    // recursive call for all children
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        find_prefix_files(tr_node->children[i], f_arr, cnt, capac);
    }
}

// function that frees the memory of one single trie node
// recursive function, also frees the memory of all children
// also uses the function that frees all ref nodes
void free_node_trie(trie_node_t *tr_node) {
    if (tr_node == NULL) {
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (tr_node->children[i] != NULL) {
            free_node_trie(tr_node->children[i]);
        }
    }

    free_refs(tr_node->refs);
    free(tr_node->children);
    free(tr_node);
}

// function that frees the memory of an entire trie
// so, with other words, frees node root of trie
void free_trie(trie_t *trie)
{
    if (trie == NULL) {
        return;
    }

    free_node_trie(trie->root);
    free(trie);
}
