#include "hashtbl.h"
#include <stdio.h>

unsigned int hash(const char *key)
{
    unsigned int count = 0;
    for(unsigned int i = 0; key[i];count += key[i], i++);
    return count;
}


void hashtbl_insert(struct hashtbl *table, const char *key, void *value)
{
    int offset = hash(key) % 32;
    if (table->keys[offset]) {
		if (strcmp(table->keys[offset], key) == 0 ) {
			table->values[offset] = value;
		} else {
			if (!table->next)
			{
				table->next = malloc(sizeof(struct hashtbl));
				*table->next = (struct hashtbl){.next = NULL};
			}
			hashtbl_insert(table->next, key, value);
		}
    } else {
        table->values[offset] = value;
        int key_len = strlen(key);
        printf("length of %s is %d", key, key_len);
        (table->keys[offset]) = malloc(key_len+1);
        snprintf(table->keys[offset], key_len+1, "%s", key);
        printf("NEW KEY %s\n", table->keys[offset]);
    }
}

void *hashtbl_find(struct hashtbl *table, const char *key)
{
    int offset = hash(key) % 32;
    if (!table) {
        return NULL;
    }
    if (table->keys[offset]) {
		if (strcmp(table->keys[offset], key) == 0 ) {
		    return table->values[offset];
		} else {
			if (table->next)
			{
				return hashtbl_find(table->next, key);
			}
		}
	}
    return NULL;
}
