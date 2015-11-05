#include "enemy.h"

#include <chipmunk/chipmunk.h>

#include "obj.h"
#include "collisiontypes.h"

void initialize_enemy(struct obj *self)
{
    cpBodyEachShape(self->body, shape_set_collision_type, CT_ENEMY);
}
