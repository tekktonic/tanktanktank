#include "jsonutils.h"
SDL_Rect from_json_rect(json_t *rect)
{
    SDL_Rect ret = {.x = json_integer_value(json_object_get(rect, "x")), .y = json_integer_value(json_object_get(rect, "y")), .w = json_integer_value(json_object_get(rect, "w")), .h = json_integer_value(json_object_get(rect, "h"))};
    return ret;
}

void from_json_circle(json_t *circle, int *x, int *y)
{
    *x = json_integer_value(json_object_get(circle, "x"));
    *y = json_integer_value(json_object_get(circle, "y"));
}
