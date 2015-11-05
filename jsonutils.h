#ifndef JSON_UTILS_H
#define JSON_UTILS_H
#include <jansson.h>
#include <chipmunk/chipmunk.h>
#include <SDL2/SDL.h>

#define CHECK_JSON(obj, e) {if (!obj) { fprintf(stderr, "%s:%d:%d %s\n", e.source, e.line, e.column, e.text); exit(1);}}
#define OPEN_JSON(file, root, err) {root = json_load_file(file,0,&err);CHECK_JSON(root,err);assert(json_is_object(root));}

void from_json_circle(json_t *circle, int *x, int *y);
#endif
