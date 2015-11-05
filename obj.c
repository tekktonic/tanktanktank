#include <stdbool.h>
#include <assert.h>
#include <chipmunk/chipmunk.h>

#include "obj.h"
#include "tonic.h"
#include "hashtbl.h"
#include "jsonutils.h"
#include "player.h"
#include "bullet.h"

#include "globals.h"
#include "hint.h"
#include "room.h"

void methodtbl_insert(struct methodtbl *table, const char *key, objmethod method)
{
    int offset = hash(key) % 32;
    if (table->keys[offset]) {
		if (strcmp(table->keys[offset], key) == 0 ) {
			table->methods[offset] = method;
		} else {
			if (!table->next)
			{
				table->next = malloc(sizeof(struct methodtbl));
				*table->next = (struct methodtbl){.next = NULL};
			}
			methodtbl_insert(table->next, key, method);
		} 
	} else {
        table->methods[offset] = method;
        int key_len = strlen(key);
//        printf("length of %s is %d", key, key_len);
        (table->keys[offset]) = malloc(key_len+1);
        snprintf(table->keys[offset], key_len+1, "%s", key);
//        printf("NEW KEY %s\n", table->keys[offset]);
    }
}


objmethod methodtbl_find(struct methodtbl *table, const char *key)
{
    int offset = hash(key) % 32;

    if (!table) {
        return NULL;
    }
    
    if (table->keys[offset]) {
		if (strcmp(table->keys[offset], key) == 0 ) {
			return table->methods[offset];
		}
        methodtbl_find(table->next, key);
    }
    return NULL;
}



size_t obj_vector_append(struct obj_vect *vect, struct obj *object)
{
	unsigned int i;

	for(i = 0; i < vect->capacity; i++)
	{
	    if (!vect->items[i]) {
            vect->items[i] = object;
            return i;
	    }
	}
	size_t new_capacity = vect->capacity * 2;
	vect->items = realloc(vect->items, sizeof(struct obj*) * new_capacity);
	memset(vect->items+vect->capacity, 0, (new_capacity - vect->capacity) * sizeof(struct obj*));
	vect->items[vect->capacity] = object;


	vect->capacity = new_capacity;
	return i;
}

struct obj *obj_vector_delete(struct obj_vect *vect, struct obj *object)
{
    for(unsigned int i = 0; i < vect->capacity; i++)
        if (vect->items[i] == object)
            vect->items[i] = NULL;
    return NULL;
}

struct obj *instantiate_object(const char *file, struct obj_vect *objects, struct room *room)
{
    struct obj *rt = malloc(sizeof(struct obj));
    struct obj ret;
    json_t *root;
    json_error_t error;
//    fprintf(stderr, "FILE IS %s\n", file);

    root = load_object(file);

    ret.room = room;
    ret.definition = root;
    ret.tex = load_texture(room->r, json_string_value(json_object_get(root, "texture")));

    {
        cpFloat mass, radius;
        int x, y;
        if (get_hint(root, "obj", "position"))
        {
            from_json_circle(get_hint(root, "obj", "position"), &x, &y);
        }
        else
        {
            x = y = 500;
        }

        
        radius = get_hint_double(root, "obj", "radius", 8.0);
        mass = get_hint_double(root, "obj", "mass", 1.0);

        ret.radius = radius;
        ret.body = cpSpaceAddBody(room->space, cpBodyNew(mass, cpMomentForCircle(0.000000004, 0, radius*2, cpvzero)));

        cpBodySetPosition(ret.body, cpv(x, y));
        ret.shape = cpSpaceAddShape(room->space, cpCircleShapeNew(ret.body, radius, cpvzero));
        cpShapeSetFriction(ret.shape, 10.0);
        cpShapeSetElasticity(ret.shape, 1.0);
    }

    ret.extra = NULL;
    ret.renderer = room->r;


    char *constructor = json_string_value(json_object_get(root, "constructor"));
    if (constructor) {
        objmethod cons = methodtbl_find(&constructors, constructor);
        if (cons) {
            cons(&ret);
        }
        free(constructor);
    }
    
    char *update = json_string_value(json_object_get(root, "update"));
    if (update) {
//        printf("%s\n", update);
        objmethod upd = methodtbl_find(&updaters, update);
        ret.update = methodtbl_find(&updaters, update);
        free(update);
    } else {
            ret.update = NULL;
    }

    
    const char *react = json_string_value(json_object_get(root, "react"));
    if(react) {
    if (strcmp(react, "die") == 0) {
        ret.react = die;
    } else {
        ret.react = NULL;
    }
    
        free(react);
    } else {
        ret.react = NULL;
    }
    
    ret.objectVect = objects;
    ret.type = (char*)json_string_value(json_object_get(root, "type"));
    if (!hashtbl_find(&objectTypes, ret.type)) {
        struct obj_vect *objs = malloc(sizeof(struct obj_vect));
        *objs = (struct obj_vect) {.capacity = 32};
        objs->items = calloc(32, sizeof(struct obj*));

        hashtbl_insert(&objectTypes, ret.type, objs);
    }
    *rt = ret;
    rt->typeOffset = obj_vector_append(hashtbl_find(&objectTypes, ret.type), rt);
    rt->objectOffset = obj_vector_append(objects, rt);
    
    char *gunType = get_hint_string(root, "obj", "gun", "machine gun");
    char *bulletType = get_hint_string(root, "obj", "bullet", "bullet.json");

    rt->gun = instantiate_gun(rt, gunType, bulletType); // needs to wait until rt has the room pointer

    free(gunType);
    free(bulletType);
    
    cpBodySetUserData(rt->body, rt);
    return rt;
}

void print_obj(struct obj object)
{
    printf("{tex: %p, update: %p renderer: %p, definition: %p, extra: %p}\n", (void*)object.tex, (void*)object.update, (void*)object.renderer, (void*)object.definition, (void*)object.extra);
}


void die(struct obj *self, struct obj *other) // assume other takes care of itself
{
    struct obj_vect *type = hashtbl_find(&objectTypes, self->type);
    obj_vector_delete(type, self);
    obj_vector_delete(self->objectVect, self);
    *self = (struct obj){.tex = NULL}; // null everything out
    struct obj *tof = self;
    self = NULL;
    free(tof);
}

struct methodpair {
    const char *name;
    const objmethod method;
};

#include "player.h"
#include "bullet.h"
#include "enemy.h"

void register_constructors(void)
{
    struct methodpair constructorPairs[] = {
        {"player", initialize_player},
        {"bullet", initialize_bullet},
        {"enemy", initialize_enemy},
    };

    
    for(size_t i = 0; i < ARRAY_LENGTH(constructorPairs); i++) {
        char *currentName = constructorPairs[i].name;
        objmethod currentMethod = constructorPairs[i].method;

        if ((!currentName && currentMethod) || (currentName && !currentMethod)) {
            CROAK("Unevenly sized methodpair array");
        }
        methodtbl_insert(&constructors, currentName, currentMethod);
    }
}

void register_updaters(void)
{
    struct methodpair updaterPairs[] = {
        {"player", player_update},
        {"bullet", bullet_update},
    };

    
    for(size_t i = 0; i < ARRAY_LENGTH(updaterPairs); i++) {
        char *currentName = updaterPairs[i].name;
        objmethod currentMethod = updaterPairs[i].method;

        if ((!currentName && currentMethod) || (currentName && !currentMethod)) {
            CROAK("Unevenly sized constructor array");
        }
        methodtbl_insert(&updaters, currentName, currentMethod);
    }
}
