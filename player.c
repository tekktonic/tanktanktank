#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "obj.h"
#include "player.h"
#include "extras.h"
#include "hint.h"
#include "hashtbl.h"
#include "globals.h"
#include "room.h"
#include "collisiontypes.h"

void initialize_player(struct obj *self)
{
        Mix_Chunk *sound = load_sound("electric_1.wav");
        CHECK_EXTRA_STRUCT(self,sizeof(struct player_extra), struct player_extra, .keystate = SDL_GetKeyboardState(NULL), .gunSound = sound);


    cpBodyEachShape(self->body, shape_set_collision_type, CT_PLAYER);

    printf("old volume: %d", sound, -1);
    self->room->viewport.focus = self->body;
    self->room->viewport.focusLast = cpBodyGetPosition(self->body);
}

void player_update(struct obj *self)
{
    struct player_extra extra_ro = *(struct player_extra*)self->extra;
    struct player_extra *extra_rw = (struct player_extra*)self->extra;
#if INPUT == KEYBOARD
    char *keys = extra_ro.keystate;

    #define RT 0.000000009
    if (keys[SDL_SCANCODE_X]) {
        if (self->gun.fire) {
            Mix_PlayChannel(-1, extra_ro.gunSound, 0);
            self->gun.fire(&(self->gun));
        }
    }
    if (keys[SDL_SCANCODE_Z]) {
        cpBodyApplyImpulseAtLocalPoint(self->body, cpv(-RT, 0.0), cpv(0, -32));
        cpBodyApplyImpulseAtLocalPoint(self->body, cpv(RT, 0.0), cpv(0, 32));
    }

    if (keys[SDL_SCANCODE_C]) {
//        cpBodySetVelocity(self->body, cpv(0, 0));
//        cpBodySetAngularVelocity(self->body, 0.0);
    }
   if (keys[SDL_SCANCODE_V]) {
        cpBodyApplyImpulseAtLocalPoint(self->body, cpv(RT, 0.0), cpv(0, -32));
        cpBodyApplyImpulseAtLocalPoint(self->body, cpv(-RT, 0.0), cpv(0, 32));
    }

/*#elif INPUT == CONTROLLER
//	SDL_GameControllerUpdate();
    Sint16 cx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);

    if (abs(cx) < 6000)
        cx = 0;
    double dx = (abs(cx) / (double)32767.0) * (cx > 0 ? 1 : -1);
    Sint16 cy = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
    if (abs(cy) < 6000)
        cy = 0;
    double dy = (abs(cy) / (double)32767.0) * (cy > 0 ? 1 : -1);

//	printf("%d %f %d %f\n", cx, dx, cy, dy);
    self->position.y += (speed * dy);
    self->position.x += (speed * dx);
    }*/
    #endif
}
