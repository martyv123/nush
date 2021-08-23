#ifndef LIST_H
#define LIST_H

/*
 * This code was taken from Prof. Nat Tuck's lecture: 
 * https://github.com/NatTuck/scratch-2021-01/blob/master/3650-02/0219/calc/list.h
 */

// An empty list is a null pointer.
typedef struct list {
    char* head;
    struct list* tail;
} list;

list* cons(char* xx, list* xs);
void free_list(list* xs);
void print_list(list* xs);
long length(list* xs);
list* reverse(list* xs);
list* rev_free(list* xs);
int size(list* xs);
int has(char* xx, list* xs);

#endif
