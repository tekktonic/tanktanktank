#ifndef COLLISION_TYPES
#define COLLISION_TYPES
#include <chipmunk/chipmunk.h>

enum CollisionTypes {
    CT_NOTUSED,
    CT_WALL,
    CT_PLAYER,
    CT_ENEMY,
    CT_BULLET
};

void shape_set_collision_type(cpBody *body, cpShape *shape, void *data);
#endif
