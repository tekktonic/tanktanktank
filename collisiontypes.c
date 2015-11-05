#include "collisiontypes.h"

void shape_set_collision_type(cpBody *body, cpShape *shape, void *data)
{
    enum CollisionTypes ct = (enum CollisionTypes)data;
    cpShapeSetCollisionType(shape, ct);
}
