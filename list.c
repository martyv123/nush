#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

/*
 * This code was taken from Prof. Nat Tuck's lecture: 
 * https://github.com/NatTuck/scratch-2021-01/blob/master/3650-02/0219/calc/list.c
 */

int
has(char* xx, list* xs)
{
    int has = 0;
    for (; xs; xs = xs->tail) {
        if (strcmp(xs->head, xx) == 0) {
            has = 1;
        }
    }
    return has;
}

int
size(list* xs)
{
    int size = 0;
    for (; xs; xs = xs->tail) {
        size += 1;
    }
    
    return size;
}

list*
cons(char* xx, list* xs)
{
    list* ys = malloc(sizeof(list));
    ys->head = strdup(xx);
    ys->tail = xs;
    return ys;
}

void
free_list(list* xs)
{
    if (xs) {
        free_list(xs->tail);
        free(xs->head);
        free(xs);
    }
}

void
print_list(list* xs)
{
    for (; xs; xs = xs->tail) {
        printf(" - %s\n", xs->head);
    }
}

long
length(list* xs)
{
    long yy = 0;
    for (; xs; xs = xs->tail) {
        yy++;
    }
    return yy;
}

list*
reverse(list* xs)
{
    list* ys = 0;
    for (; xs; xs = xs->tail) {
        ys = cons(xs->head, ys);
    }
    return ys;
}

list*
rev_free(list* xs)
{
    list* ys = reverse(xs);
    free_list(xs);
    return ys;
}
