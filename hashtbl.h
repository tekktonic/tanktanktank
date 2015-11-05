#ifndef HASHTBL_H
#define HASHTBL_H
#include <stdlib.h>
#include <string.h>

unsigned int hash(const char *key);

#define HASHTBL_SIZE 32
struct hashtbl
{
    void *values[HASHTBL_SIZE];
    char *keys[HASHTBL_SIZE];
    struct hashtbl *next;
};

void hashtbl_insert(struct hashtbl *table, const char *key, void *value);

void *hashtbl_find(struct hashtbl *table, const char *key);

#endif
