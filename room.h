#ifndef ROOM_H
#define ROOM_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "obj.h"
#include "hashtbl.h"
#include "viewport.h"

#include <chipmunk/chipmunk.h>
struct spawn_list
{
    struct spawn_list *last;
    const char *object;
    long long spawn_tick;
    json_t *extra;
    struct spawn_list *next;
};

struct room
{
    cpSpace *space;
    struct obj_vect *objects;
    cpBody *wallBody;
    cpShape *walls[4];

    struct hashtbl *layers;
    struct spawn_list *to_spawn;
    struct viewport viewport;
    SDL_Renderer *r;
    SDL_Texture *background;
    
    Mix_Music *music;
    void (*update)(struct room*);
    long long ticks;
};

struct room *instantiate_room(const char *filename, SDL_Renderer *r);
void room_update(struct room *self);
#endif
