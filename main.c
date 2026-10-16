/* NICOLESCU David-Stefan - 313CA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Copyright devmind lab
#define DIE(assertion, call_description)                                      \
	do {                                                                      \
		if (assertion) {                                                      \
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                \
			perror(call_description);                                         \
			exit(errno);                                                      \
		}                                                                     \
	} while (0)

#include "file.h"
#include "trie.h"
#include "heap.h"

#define MAX_LEN 256

// helper printing functions
void function_print_ok(FILE *fout)
{
    fprintf(fout, "OK\n");
}

void func_print_exists(FILE *fout) 
{
    fprintf(fout, "EXISTS\n");
}

void func_print_not_found(FILE *fout) {
    fprintf(fout, "NOT FOUND\n");
}

void print_invalid_comm(FILE *fout) {
    fprintf(fout, "Not a valid command!\n");
}

// ADD command, creates a new file, and adds all new T keywords
void add_command(FILE *fin, FILE *fout, file_dllist_t *files, trie_t *trie) {
    char id[MAX_LEN], curr_kw[MAX_LEN];
    int rel_scor, kw_count;

    fscanf(fin, "%255s%d%d", id, &rel_scor, &kw_count);

    // the ID already exists
    if (find_file(files, id) != NULL) {
        for (int i = 0; i < kw_count; i++) {
            fscanf(fin, "%255s", curr_kw);
        }
        func_print_exists(fout);
        return;
    }
    
    // adding the new file
    file_node_t *new_file = create_file(id, rel_scor);
    add_file_to_list(files, new_file);

    // adding each keyword to file and trie
    for (int i = 0; i < kw_count; i++) {
        fscanf(fin, "%255s", curr_kw);
        if (!file_has_kw(new_file, curr_kw)) {
            add_keyword_to_file(new_file, curr_kw);
            insert_word_trie(trie, trie->root, curr_kw, new_file);
        }
    }
    function_print_ok(fout);
}

// helper function for DEL command
void total_delete_file(file_dllist_t *list, trie_t *trie, file_node_t *file) {
    if (list == NULL || trie == NULL || file == NULL) {
        return;
    }

    // auxiliary array in which I'll store all keywords of given file
    char **copy;
    copy = malloc(file->kw_count * sizeof(*copy));
    DIE(copy == NULL, "malloc copy delete file");

    for(int i = 0; i < file->kw_count; i++) {
        copy[i] = custom_strdup(file->keywords[i]);
    }

    // for each keyword, delete the ref node for the file
    // + update trie, if it was the only ref node of a trie node
    for (int i = 0; i < file->kw_count; i++) {
        trie_node_t *tr_node = find_word_trie(trie, copy[i]);

        if (tr_node != NULL) {
            remove_ref_trie(tr_node, file);
            del_word_if_empty_trie(trie, copy[i]);
        }
    }

    for(int i = 0; i < file->kw_count; i++) {
        free(copy[i]);
    }

    free(copy);
    remove_file_from_list(list, file);
    free_file(file);
}

// DEL command
void del_command(FILE *fin, FILE *fout, file_dllist_t *files, trie_t *trie) {
    char id[MAX_LEN];

    fscanf(fin, "%255s", id);

    file_node_t *file_to_del = find_file(files, id);

    // given file doesn't exist
    if (file_to_del == NULL) {
        func_print_not_found(fout);
        return;
    }
    
    // helper function for delete
    total_delete_file(files, trie, file_to_del);

    function_print_ok(fout);
}

// ADDKW command, adds a keyword to a existing file
void addkw_command(FILE *fin, FILE *fout, file_dllist_t *list, trie_t *trie) {
    char id[MAX_LEN], keyword[MAX_LEN];

    fscanf(fin, "%255s%255s", id, keyword);

    file_node_t *wanted_file = find_file(list, id);

    // case: file doesn't exist
    if (wanted_file == NULL) {
        func_print_not_found(fout);
        return;
    }

    // case: file already has that keyword
    if (file_has_kw(wanted_file, keyword) == 1) {
        function_print_ok(fout);
        return;
    }

    // adds keyword to file
    add_keyword_to_file(wanted_file, keyword);
    // updates trie
    insert_word_trie(trie, trie->root, keyword, wanted_file);
    function_print_ok(fout);
}

// DELKW command, deletes a keyword from an existent file
void delkw_command(FILE *fin, FILE *fout, file_dllist_t *list, trie_t *trie) {
    char id[MAX_LEN], keyword[MAX_LEN];

    fscanf(fin, "%255s%255s", id, keyword);

    // find needed file
    file_node_t *wanted_file = find_file(list, id);

    // case: the file doesn't exist in the list
    if (wanted_file == NULL) {
        func_print_not_found(fout);
        return;
    }

    // the given file doesn't have the keyword to delete
    if (file_has_kw(wanted_file, keyword) == 0) {
        function_print_ok(fout);
        return;
    }

    // find terminal node of keyword in trie
    trie_node_t *tr_node = find_word_trie(trie, keyword);

    // update the trie
    if (tr_node != NULL) {
        remove_ref_trie(tr_node, wanted_file);
        del_word_if_empty_trie(trie, keyword);
    }

    // deletes keyword from file
    remove_kw_from_file(wanted_file, keyword);

    // if the file doesn.t have any more keywords, delete file
    if (wanted_file->kw_count == 0) {
        remove_file_from_list(list, wanted_file);
        free_file(wanted_file);
    }

    function_print_ok(fout);
}

// FIND command, given a keyword, prints all files that contain that specified
// keyword
void find_command(FILE *fin, FILE *fout, trie_t *trie) {
    char word[MAX_LEN];

    fscanf(fin, "%255s", word);

    print_for_find_command(trie, word, fout);
}

// TOPK command, given a word, find the first k most relevant files that
// contain keyword; task that uses heap
void topk_command(FILE *fin, FILE *fout, trie_t *trie)
{
    char word[MAX_LEN];
    int k, printed = 0, heap_dim;
    heap_t *heap;

    fscanf(fin, "%255s%d", word, &k);

    trie_node_t *tr_node = find_word_trie(trie, word);

    // edge case
    if (tr_node == NULL || tr_node->refs == NULL || k <= 0) {
        print_empty_message(fout);
        return;
    }

    heap_dim = count_ref_trie(tr_node);
    heap = create_heap(heap_dim);

    ref_trie_node_t *ref_node = tr_node->refs;

    // adding all files in the heap
    while (ref_node != NULL) {
        push_heap(heap, ref_node->file);
        ref_node = ref_node->next;
    }

    // if there are fewer than k files, we print them all
    if (k > heap_dim) {
        k = heap_dim;
    }

    fprintf(fout, "%d", k);

    // print first most relevant files
    while (printed < k) {
        file_node_t *pr_file = pop_heap(heap);

        fprintf(fout, " %s", pr_file->id);
        printed++;
    }
    fprintf(fout, "\n");

    free_heap(heap);
}

// BONUS PREFIX task, we print all files that have the keywords that start
// with prefix
void prefix_command(FILE *fin, FILE *fout, trie_t *trie) {
    char prefix[MAX_LEN];

    fscanf(fin, "%255s", prefix);

    // the terminal node of prefix in trie
    trie_node_t *tr_node = find_prefix_node(trie, prefix);
    int cnt = 0, capacity = 8; // initial capacity
    // arrayfor all the files
    file_node_t **file_array;

    // edge case
    if (tr_node == NULL) {
        print_empty_message(fout);
        return;
    }

    file_array = malloc(capacity * sizeof(*file_array));
    DIE(file_array == NULL, "malloc file array prefix command");

    // collects all files and stores them in the array
    find_prefix_files(tr_node, &file_array, &cnt, &capacity);

    // case: no files found
    if (cnt == 0) {
        print_empty_message(fout);
    } else {
        fprintf(fout, "%d", cnt);
        for (int i = 0; i < cnt; i++) {
            fprintf(fout, " %s", file_array[i]->id);
        }
        fprintf(fout, "\n");
    }

    free(file_array);
}

// PRINT command, so we print the trie, or EMPTY message
void print_command(trie_t *trie, FILE *fout) {
    if (trie->size == 0) {
        print_empty_message(fout);
        return;
    }

    print_trie(trie, fout);
}

// MAIN function, we open the input/output files and read all commands
int main(void)
{
    FILE *fin = fopen("indexare.in", "r");
    FILE *fout = fopen("indexare.out", "w");
    DIE(fin == NULL, "fopen indexare in");
    DIE(fout == NULL, "fopen indexare out");

    int nr_of_operations;
    char curr_op[MAX_LEN];

    fscanf(fin, "%d", &nr_of_operations);

    // we create our files list and trie
    file_dllist_t *the_x_files = create_file_list();
    trie_t *the_trie =  trie_create();

    // for each command, read the first word and identify the respective task
    for (int i = 0; i < nr_of_operations; i++) {
        fscanf(fin, "%255s", curr_op);

        if (strcmp(curr_op, "ADD") == 0) {
            add_command(fin, fout, the_x_files, the_trie);
        } else if (strcmp(curr_op, "DEL") == 0) {
            del_command(fin, fout, the_x_files, the_trie);
        } else if (strcmp(curr_op, "ADDKW") == 0) {
            addkw_command(fin, fout, the_x_files, the_trie);
        } else if (strcmp(curr_op, "DELKW") == 0) {
            delkw_command(fin, fout, the_x_files, the_trie);
        } else if (strcmp(curr_op, "FIND") == 0) {
            find_command(fin, fout, the_trie);
        } else if (strcmp(curr_op, "TOPK") == 0) {
            topk_command(fin, fout, the_trie);
        } else if (strcmp(curr_op, "PRINT") == 0) {
            print_command(the_trie, fout);
        } else if (strcmp(curr_op, "PREFIX") == 0) {
            prefix_command(fin, fout, the_trie);
        } else {
            // although not mentioned in pdf, for completeness
            print_invalid_comm(fout);
        }
    }

    // freeing all used memory
    free_trie(the_trie);
    free_list_of_files(the_x_files);

    // closing files
    fclose(fin);
    fclose(fout);
    return 0;
}