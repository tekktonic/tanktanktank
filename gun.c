#include <chipmunk/chipmunk.h>

#include "gun.h"
#include "obj.h"
#include "room.h"

#include "tonic.h"
#include "extras.h"

struct guntbl fires;

struct gun instantiate_gun(struct obj *owner, const char *gunType,
                           const char *bulletType)
{
//    fprintf(stderr, "\n\n\n%s\n\n\n", bulletType);
    struct gun ret = {.bulletType = strdup(bulletType), .owner = owner, .cooldown = owner->room->ticks};
    ret.fire =  guntbl_find(&fires, gunType);
    return ret;
}

void machinegun_fire(struct gun *self)
{
    if (self->owner->room->ticks - self->cooldown > 1) {
        struct obj *bullet = instantiate_object(self->bulletType,
                                                self->owner->objectVect,
                                                self->owner->room);

        cpBodySetPosition(bullet->body,
                          cpBodyLocalToWorld(self->owner->body,
                                             cpv(0, -self->owner->radius - bullet->radius - 2)));

        cpBodySetMoment(bullet->body,
                        cpMomentForCircle(10000.0, 0.0,
                                          bullet->radius*2, cpvzero));
        cpBodySetAngle(bullet->body, cpBodyGetAngle(self->owner->body) + M_PI);
        ((struct bullet_extra*)bullet->extra)->speed = 20.0;
        ((struct bullet_extra*)bullet->extra)->owner = self->owner;

        cpBodyApplyImpulseAtLocalPoint(self->owner->body,
                                       cpv(0, 50.0),
                                       cpv(0, -(self->owner->radius * 2)));
        
        self->cooldown = self->owner->room->ticks;
    }
}
struct gunpair {
    const char *name;
    const gunmethod method;
};

void register_guns(void)
{
    struct gunpair pairs[] = {{"machine gun", machinegun_fire}};

    for (size_t i = 0; i < ARRAY_LENGTH(pairs); i++) {
        char *currentName = pairs[i].name;
        gunmethod currentMethod = pairs[i].method;

        if ((!currentName && currentMethod) || (currentName && !currentMethod)) {
            CROAK("Unevenly sized methodpair array");
        }
        guntbl_insert(&fires, currentName, currentMethod);
    }
}


void guntbl_insert(struct guntbl *table, const char *key, gunmethod method)
{
    int offset = hash(key) % 32;
    if (table->keys[offset]) {
		if (strcmp(table->keys[offset], key) == 0 ) {
			table->methods[offset] = method;
		} else {
			if (!table->next)
			{
				table->next = malloc(sizeof(struct guntbl));
				*table->next = (struct guntbl){.next = NULL};
			}
			guntbl_insert(table->next, key, method);
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


gunmethod guntbl_find(struct guntbl *table, const char *key)
{
    int offset = hash(key) % 32;

//    fprintf(stderr, "\n\n\nkey %s\n\n\n", key);
    if (!table) {
        return NULL;
    }
    
    if (table->keys[offset]) {
		if (strcmp(table->keys[offset], key) == 0 ) {
			return table->methods[offset];
		}
        guntbl_find(table->next, key);
    }
    return NULL;
}
