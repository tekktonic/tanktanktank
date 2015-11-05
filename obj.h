#ifndef OBJ_H
#define OBJ_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <jansson.h>

#include <chipmunk/chipmunk.h>

#include "gun.h"

struct room;

struct obj
{
    SDL_Texture *tex;
    cpBody *body;
    cpShape *shape;
    double radius;
    struct room *room;
    char *type;
    struct gun gun;
    
    SDL_Renderer *renderer;


    void (*update)(struct obj*);
    void (*react)(struct obj*, struct obj*);
    
    json_t *definition;
    struct obj_vect *typeVect, *objectVect;
    size_t typeOffset, objectOffset;

    void *extra;
};

struct obj_vect
{
    struct obj **items;
	size_t capacity;
};

struct obj *instantiate_object(const char *file, struct obj_vect *objects, struct room *room);


void die(struct obj *self, struct obj *other); // assume other takes care of itself


typedef void (*objmethod)(struct obj*);

struct methodtbl
{
    objmethod methods[32];
    char *keys[32];
    struct methodtbl *next;
};

void methodtbl_insert(struct methodtbl *table, const char *key, void (*value)(struct obj *));

objmethod methodtbl_find(struct methodtbl *table, const char *key);

size_t obj_vector_append(struct obj_vect *vect, struct obj *object);

struct obj *obj_vector_delete(struct obj_vect *vect, struct obj *to_delete);
//size_t obj_vector_len(struct obj_vect *vect);


void print_obj(struct obj object);

struct methodtbl constructors;
struct methodtbl updaters;
struct methodtbl reactors;

void register_constructors(void);
void register_updaters(void);
void register_reactors(void);

#endif
