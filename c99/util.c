/* util.c - Utility functions for SDS converter.
 * Copyright (C) 2011 Matthew Bishop
 */
#include "util.h"
#include <stdio.h>
#include <string.h>

void *xmalloc(size_t bytes)
{
    void *p = malloc(bytes);
    if (!p) {
        fprintf(stderr, "Failed to malloc(%u)\n", (unsigned)bytes);
        abort();
    }
    return p;
}

void *xcalloc(size_t bytes)
{
    void *p = calloc(1, bytes);
    if (!p) {
        fprintf(stderr, "Failed to calloc(1, %u)\n", (unsigned)bytes);
        abort();
    }
    return p;
}

void *xrealloc(void *ptr, size_t bytes)
{
    void *p = realloc(ptr, bytes);
    if (!p) {
        fprintf(stderr, "Failed to realloc(%p, %u)\n", ptr, (unsigned)bytes);
        abort();
    }
    return p;
}

/* Grows the given array by doubling its capacity.  Returns the
 * pointer to the new memory block.  ary: the array to grow.
 * element_size: the size of each element in the array.  capacity: an
 * in/out parameter containing the current capacity of the array.  It
 * will be doubled before the function returns.
 */
void *grow_ary(void *ary, size_t element_size, size_t *capacity)
{
    if (*capacity == 0)
        *capacity = 1;
    else
        *capacity *= 2;
    return realloc(ary, element_size * *capacity);
}

char *xstrdup(const char *s)
{
    char *t = xmalloc(strlen(s) + 1);
    strcpy(t, s);
    return t;
}

static List *list_r(List *prev, List *l)
{
    List *next = l->next;
    l->next = prev;
    return next ? list_r(l, next) : l;
}

size_t list_count(List *l)
{
    size_t n = 0;
    while (l) {
        n++;
        l = l->next;
    }
    return n;
}

List *list_reverse(List *l)
{
    if (!l) return NULL;
    return list_r(NULL, l);
}

List *list_find(List *l, char *key)
{
    for (; l != NULL; l = l->next) {
        if (!strcmp(key, l->key))
            return l;
    }
    return NULL;
}
