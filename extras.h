#ifndef EXTRAS_H
#define EXTRAS_H
#define CHECK_EXTRA(obj, size, typ, rval) {if(obj->extra == NULL){obj->extra = malloc(size);*(typ*)obj->extra = rval;}}
#define CHECK_EXTRA_STRUCT(obj, size, typ, ...) {if(obj->extra == NULL){obj->extra = malloc(size);(*(typ *)obj->extra) = (typ){__VA_ARGS__};}}



struct player_extra
{
    char *keystate;
    Mix_Chunk *gunSound;
};

#include "obj.h"
struct bullet_extra
{
    struct obj *owner;
    void (*behavior)(struct obj*);
    double speed;
};

#endif
