#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
// NO HEADERS MAY INCLUDE THIS FILE
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <jansson.h>

#include "hashtbl.h"
#include "obj.h"
#include "tonic.h"



void init_resource_caches(void);


long long numBullets;

SDL_Texture *load_texture(SDL_Renderer *r, const char *filename);
void release_textures(void);

Mix_Music *load_song(const char *filename);
void release_songs(void);

Mix_Chunk *load_sound(const char *filename);
void release_sounds(void);

json_t *load_level(const char *filename);
void release_levels(void);

json_t *load_object(const char *filename);
void release_objects(void);
struct hashtbl objectTypes;

void free_shape_each(cpBody *body, cpShape *shape, void *data);


bool point_rect_collide(int x, int y, SDL_Rect rect);
#endif
