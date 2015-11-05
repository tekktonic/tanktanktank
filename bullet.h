#ifndef BULLET_H
#define BULLET_H
#include <chipmunk/chipmunk.h>
#include "obj.h"

void initialize_bullet(struct obj *self);
void bullet_update(struct obj *self);
void bullet_free_cb(cpSpace *space, void *self, void *data);
void bullet_wall_cb(cpArbiter *arb, cpSpace *space, cpDataPointer ptr);
void bullet_enemy_cb(cpArbiter *arb, cpSpace *space, cpDataPointer ptr);
#endif
