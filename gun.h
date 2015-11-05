#ifndef GUN_H
#define GUN_H

#include <jansson.h>
struct obj;
struct room;



struct gun {
    const char *bulletType;
    struct obj *owner;
    long long cooldown;
    void (*fire)(struct gun*);
};

typedef void (*gunmethod)(struct gun*);
struct gun instantiate_gun(struct obj *owner, const char *gunType, const char *bulletType);



void machinegun_fire(struct gun *self);

struct guntbl
{
    gunmethod methods[32];
    char *keys[32];
    struct guntbl *next;
};

void guntbl_insert(struct guntbl *table, const char *key, gunmethod value);

gunmethod guntbl_find(struct guntbl *table, const char *key);

void register_guns(void);
#endif
