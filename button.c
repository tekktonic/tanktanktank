#include "extras.h"
#include "globals.h"
#include "buttonh.h"

struct widget *initialize_button(struct widget *self)
{
    self->update = button_update;

}
void button_update(struct widget *self)
{
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    if (point_rect_collide(mouse_x, mouse_y, self->position))
}
