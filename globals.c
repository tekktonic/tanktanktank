#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


#include "hashtbl.h"
#include "jsonutils.h"

#include "player.h"
#include "bullet.h"
#include "enemy.h"

#include "globals.h"

long long numBullets = 0;

static const char *imageDirectory = "resources/images/";
static const char *songDirectory = "resources/music/";
static const char *soundDirectory = "resources/sounds/";

static const char *levelDirectory = "resources/levels/";
static const char *objectDirectory = "resources/entities/";

static struct hashtbl images;


static struct hashtbl sounds;
static struct hashtbl songs;

static struct hashtbl levels;

SDL_Texture *load_texture(SDL_Renderer *r, const char *filename)
{
    SDL_Texture *tex;
    tex = (SDL_Texture*)hashtbl_find(&images, filename);
    if (!tex) {
//        printf("Loading texture: %s\n", filename);
        size_t fileLen = strlen(imageDirectory) + strlen(filename) + 1;
        char *file = malloc(fileLen);
        assert(file);
        snprintf(file, fileLen, "%s%s", imageDirectory, filename);
        tex = IMG_LoadTexture(r, file);
        hashtbl_insert(&images, filename, tex);
    }
    return tex;
}

void release_textures(void)
{
    struct hashtbl *current = &images;

    while(current)
    {
        for(int i = 0; i < HASHTBL_SIZE; i++)
            if (current->values[i]) {
                SDL_DestroyTexture((SDL_Texture*)current->values[i]);
                current->values[i] = NULL;
            }
        current = current->next;
    }
}

Mix_Music *load_song(const char *filename)
{
    Mix_Music *song;
    song = (Mix_Music*)hashtbl_find(&songs, filename);
    if (!song) {
//        printf("Loading song: %s\n", filename);
        size_t fileLen = strlen(songDirectory) + strlen(filename) + 1;
        char *file = malloc(fileLen);
        assert(file);
        snprintf(file, fileLen, "%s%s", songDirectory, filename);
        song = Mix_LoadMUS(file);
        hashtbl_insert(&songs, filename, song);
    }
    return song;
}

void release_songs(void)
{
    struct hashtbl *current = &songs;

    while(current)
    {
        for(int i = 0; i < HASHTBL_SIZE; i++)
            if (current->values[i]) {
                Mix_FreeMusic((Mix_Music*)current->values[i]);
                current->values[i] = NULL;
            }
        current = current->next;
    }
}

Mix_Chunk *load_sound(const char *filename)
{
    Mix_Chunk *sound;
    sound = (Mix_Chunk*)hashtbl_find(&songs, filename);
    if (!sound) {
//        printf("Loading song: %s\n", filename);
        size_t fileLen = strlen(soundDirectory) + strlen(filename) + 1;
        char *file = malloc(fileLen);
        assert(file);
        snprintf(file, fileLen, "%s%s", soundDirectory, filename);
        printf("%s\n", file);
        sound = Mix_LoadWAV(file);
        hashtbl_insert(&sounds, filename, sound);
    }

    if (!sound)
    {
        CROAK("%s\n", Mix_GetError());
    }
    return sound;
}

void release_sounds(void)
{
    struct hashtbl *current = &sounds;

    while(current)
    {
        for(int i = 0; i < HASHTBL_SIZE; i++)
            if (current->values[i]) {
                Mix_FreeChunk((Mix_Chunk*)current->values[i]);
                current->values[i] = NULL;
            }
        current = current->next;
    }
}

json_t *load_level(const char *filename)
{
    json_t *level;
    level = (json_t*)hashtbl_find(&levels, filename);
    if (!level) {
        json_error_t err;
//        printf("Loading level: %s\n", filename);
        size_t fileLen = strlen(levelDirectory) + strlen(filename) + 1;
        char *file = malloc(fileLen);
        assert(file);
        snprintf(file, fileLen, "%s%s", levelDirectory, filename);
        OPEN_JSON(file, level, err);

        CHECK_JSON(level, err);
        hashtbl_insert(&levels, filename, level);
    }
    return level;
}

void release_levels(void)
{
    struct hashtbl *current = &levels;

    while(current)
    {
        for(int i = 0; i < HASHTBL_SIZE; i++)
            if (current->values[i]) {
                json_decref((json_t*)current->values[i]);
                current->values[i] = NULL;
            }
        current = current->next;
    }
}

json_t *load_object(const char *filename)
{
    json_t *object;
    json_error_t err;
//    printf("Loading object: %s\n", filename);
    size_t fileLen = strlen(objectDirectory) + strlen(filename) + 1;
    char *file = malloc(fileLen);
    assert(file);
    snprintf(file, fileLen, "%s%s", objectDirectory, filename);
    OPEN_JSON(file, object, err);

    CHECK_JSON(object, err);
 
    return object;
}



/*void register_reactors(void)
{
    struct methodpair constructs[] = {
    };

    
    for(size_t i = 0; i < ARRAY_LENGTH(constructs); i++) {
        char *currentName = constructs[i].name;
        objmethod currentMethod = constructs[i].method;

        if ((!currentName && currentMethod) || (currentName && !currentMethod)) {
            CROAK("Unevenly sized constructor array");
        }
        methodtbl_insert(&constructors, currentName, currentMethod);
    }
    }*/
void free_shape_each(cpBody *body, cpShape *shape, void *data)
{
    cpSpaceRemoveShape(cpBodyGetSpace(body), shape);
    cpShapeDestroy(shape);
    cpShapeFree(shape);
}

bool point_rect_collide(int x, int y, SDL_Rect rect)
{
    return (x >= rect.x && x <= rect.x + rect.w) && (y >= rect.y && y <= rect.y + rect.h);
}
