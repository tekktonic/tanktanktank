#include <chipmunk/chipmunk.h>
#include "bullet.h"
#include "obj.h"
#include "globals.h"
#include "extras.h"
#include "collisiontypes.h"

#include "room.h"
#include "viewport.h"

void initialize_bullet(struct obj *self)
{
    CHECK_EXTRA_STRUCT(self,sizeof(struct bullet_extra), struct bullet_extra, .speed = 0);
    cpBodyEachShape(self->body, shape_set_collision_type, CT_BULLET);
    numBullets++;
}

void bullet_update(struct obj *self)
{
    cpBodyApplyImpulseAtLocalPoint(self->body, cpv(0, ((struct bullet_extra*)self->extra)->speed), cpv(0, -self->radius * 2));
    cpVect position = cpBodyGetPosition(self->body);

    if (viewport_translate_x(&self->room->viewport, position.x) > (1024+self->radius)
        || viewport_translate_x(&self->room->viewport, position.x) < (-self->radius)
        || viewport_translate_y(&self->room->viewport, position.y) > 768+self->radius
        || viewport_translate_y(&self->room->viewport, position.y) < (-self->radius)) {
          bullet_free_cb(cpBodyGetSpace(self->body), self, NULL);
    }
}

void bullet_free_cb(cpSpace *space, void *self, void *data)
{
    numBullets--;
    struct obj *bullet = (struct obj*)self;
    cpBodyEachShape(bullet->body, free_shape_each, NULL);
    cpSpaceRemoveBody(space, bullet->body);
    cpBodyDestroy(bullet->body);
    cpBodyFree(bullet->body);
    die(bullet, NULL);
}

void bullet_wall_cb(cpArbiter *arb, cpSpace *space, cpDataPointer ptr)
{
    cpBody *b, *wall;
    cpArbiterGetBodies(arb, &b, &wall);
    cpSpaceAddPostStepCallback(space, bullet_free_cb,
                               cpBodyGetUserData(b), NULL);
}

void bullet_enemy_cb(cpArbiter *arb, cpSpace *space, cpDataPointer ptr)
{
    cpBody *b, *e;
    cpArbiterGetBodies(arb, &b, &e);
    cpSpaceAddPostStepCallback(space, bullet_free_cb,
                               cpBodyGetUserData(b), NULL);
    cpSpaceAddPostStepCallback(space, bullet_free_cb,
                               cpBodyGetUserData(e), NULL);
}
