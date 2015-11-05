#include "room.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <jansson.h>
#include <chipmunk/chipmunk.h>

#include "jsonutils.h"
#include "globals.h"

#include "collisiontypes.h"

#include "bullet.h"

#include "viewport.h"

struct spawn_list *spawn_list_append(struct spawn_list *list, const char *object, long long spawn_tick, json_t *extra)
{
    struct spawn_list *last = NULL;
    struct spawn_list *current = list;
    if (current->next == NULL) // we're at the start of the list
    {
	current->object = object;
	current->spawn_tick = spawn_tick;
	current->extra = extra;

	current->next = malloc(sizeof(struct spawn_list));
	*(current->next) = (struct spawn_list){.next = NULL, .last = last};
    }
    else {
	while (current->next != NULL) {
	    last = current;
	    current = current->next;
	}

	current->next = malloc(sizeof(struct spawn_list));
	last = current;
	current = current->next;
	*current = (struct spawn_list){.object = object, .spawn_tick = spawn_tick, .extra = extra, .last = last, .next = NULL};

    }

    return current;
}


struct spawn_list *spawn_list_delete(struct spawn_list *list, struct spawn_list *to_delete)
{
    struct spawn_list *current = list;
    struct spawn_list *ret = list; // just in case
    while(current != to_delete) {
		if (!current)
			return NULL;
		current = current->next;
    }
    if (current->next)
		current->next->last = current->last;
    if (current->last)
		current->last->next = current->next;
    if (current == list)
		ret = current->next;
	if (!current->next)
		ret = NULL;
    struct spawn_list *dest = current;
    current = NULL;
    free(dest);


    return ret;
}

size_t spawn_list_len(struct spawn_list *list)
{
    size_t i;
    struct spawn_list *current = list;
    for(i = 0; current; current = current->next, i++);
    return i;
}

struct spawn_list *gen_spawn_list(json_t *objects)
{
    assert(json_is_array(objects));
    struct spawn_list *ret = malloc(sizeof(struct spawn_list));
    *ret = (struct spawn_list){.next = NULL};
    
    for(unsigned int i = 0; i < json_array_size(objects); i++) {
        json_t *obj = json_array_get(objects, i);
        json_error_t je;
        CHECK_JSON(obj, je);
        const char *filename = json_string_value(json_object_get(obj, "object"));
        long long spawn_tick = json_integer_value(json_object_get(obj, "time"));
//	printf("SPAWN TICK IS %lld\n", spawn_tick);
        json_t *extra = json_object_get(obj, "spawn_hint");
        json_incref(extra);
        spawn_list_append(ret, filename, spawn_tick, extra);
    }
    return ret;
}



void room_update(struct room *self)
{
    long long tick = self->ticks;
    {
		struct spawn_list *current = self->to_spawn;

		while(current) {
			struct spawn_list *last = current;
			current = current->next;
			if (last->object)
				if (tick >= last->spawn_tick) {
//					printf("TIME TO SPAWN %s, it is %lld and it was supposed to spawn on tick %lld\n", last->object, tick, last->spawn_tick);
					instantiate_object(last->object, self->objects, self);
					self->to_spawn = spawn_list_delete(self->to_spawn, last);
				}

		}
    }

    SDL_RenderCopy(self->r, self->background, NULL, NULL);
    struct obj_vect *objects = self->objects;
	for (unsigned int i = 0; i < objects->capacity; i++) {
        
        struct obj *current = objects->items[i];

        if (!current)
            continue;
        cpVect position = cpBodyGetPosition(current->body);
//        printf("%d %d\n", viewport_translate_x(&self->viewport, position.x), viewport_translate_y(&(self->viewport), position.y));
        SDL_Rect current_rect = {.x=viewport_translate_x(&self->viewport, position.x - current->radius),
                                 .y=viewport_translate_y(&self->viewport, position.y - current->radius),
                                 .w=current->radius*2,
                                 .h=current->radius*2};

//        SDL_Rect current_rect = {.x=position.x - current->radius, .y=position.y - current->radius, .w=current->radius*2, .h=current->radius*2};
        double rotation = cpBodyGetAngle(current->body) * (180 / M_PI);
        SDL_RenderCopyEx(self->r, objects->items[i]->tex, NULL, &current_rect, rotation, NULL, SDL_FLIP_NONE);
        if (current->update)
				current->update(current);

//            struct obj *current = objects->items[i];
            

	}
    viewport_update(&self->viewport);
    self->ticks++;
}

struct room *instantiate_room(const char *filename, SDL_Renderer *r)
{
    struct room *rt = malloc(sizeof(struct room));
    struct room ret;
    json_t *root;
    root = load_level(filename);

    ret.background = load_texture(r, json_string_value(json_object_get(root, "background")));

    ret.r = r;
    ret.viewport = (struct viewport){ .xo = 0, .yo = 0, .focus = NULL, .w = 1024, .h = 768};
    ret.space = cpSpaceNew();
    cpCollisionHandler *bulletWallHandler = cpSpaceAddCollisionHandler(ret.space, CT_BULLET, CT_WALL);

    bulletWallHandler->postSolveFunc = bullet_wall_cb;
    cpCollisionHandler *bulletEnemyHandler = cpSpaceAddCollisionHandler(ret.space, CT_BULLET, CT_ENEMY);

    bulletEnemyHandler->postSolveFunc = bullet_enemy_cb;
    cpSpaceSetDamping(ret.space, 0.3);
    ret.wallBody = cpSpaceAddBody(ret.space, cpBodyNewStatic());

    // TODO use new room width/height properties
    cpVect vects[4][2] = {{cpv(0,-20), cpv(1024,-20)}, {cpv(-20, -20), cpv(-20, 788)}, {cpv(1044, 0), cpv(1044, 768)}, {cpv(-20, 788), cpv(1044, 788)}};
/*    for(int i=0; i < 4; i++) {
        cpVect *v = vects[i];
        ret.walls[i] = cpSpaceAddShape(ret.space, cpSegmentShapeNew(ret.wallBody, v[0], v[1], 10.0));
        cpShapeSetCollisionType(ret.walls[i], CT_WALL);
        cpShapeSetFriction(ret.walls[i], 0.0);
        cpShapeSetElasticity(ret.walls[i], 0.8);
        }*/
    ret.objects = malloc(sizeof(struct obj_vect));

    *ret.objects = (struct obj_vect){.capacity = 32};
    ret.objects->items = calloc(32, sizeof(struct obj*));
//	memset(ret.objects->items, 0, 32 * sizeof(struct obj*));
    char *music = json_string_value(json_object_get(root, "music"));
//    printf("music %s\n", music);
    if (music) {
        ret.music = load_song(music);
        assert(ret.music);
        free(music);
    } else {
        ret.music = NULL;
    }
    ret.ticks = 0;
/*    ret.layers = malloc(sizeof(struct hashtbl));
 *ret.layers = (struct hashtbl){.next = NULL};*/

    const char *update = json_string_value(json_object_get(root, "update"));
    ret.update = room_update;
    free((char*)update);

    ret.to_spawn = gen_spawn_list(json_object_get(root, "objects"));

    *rt = ret;
    if (rt->music) {
//        printf("playing music\n");
        Mix_PlayMusic(rt->music, -1);
    }
    return rt;
}
