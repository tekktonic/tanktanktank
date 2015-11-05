#ifndef VIEWPORT_H
#define VIEWPORT_H
#include <chipmunk/chipmunk.h>

struct viewport {
    cpBody *focus;
    int w, h;
    int xo, yo;
    cpVect focusLast;
};

int viewport_translate_x(struct viewport *vp, int cp_x);
int viewport_translate_y(struct viewport *vp, int cp_y);

void viewport_update(struct viewport *vp);
#endif
