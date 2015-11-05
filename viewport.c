#include "viewport.h"
#include <stdio.h>
#include <math.h>

int viewport_translate_x(struct viewport *vp, int cp_x)
{
    return cp_x - vp->xo;
}

int viewport_translate_y(struct viewport *vp, int cp_y)
{
    return cp_y - vp->yo;
}

void viewport_update(struct viewport *self)
{
    if (self->focus) {
        cpVect targetPosition = cpBodyGetPosition(self->focus);
        int x = (int)targetPosition.x;
        int y = (int)targetPosition.y;

//        self->xo = x;
        self->xo += (x - (int)self->focusLast.x);
        self->yo += (y - (int)self->focusLast.y);
//        if (viewport_translate_x(self, x) > ((3*self->w)/4)) {
        printf("%d\n", x - (int)self->focusLast.x);
//            self->xo += (x - self->focusLast.x);
//        }
        self->focusLast = (cpVect){targetPosition.x, targetPosition.y};
    }
}
