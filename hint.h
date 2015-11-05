#ifndef HINT_H
#define HINT_H
#include <jansson.h>
json_t *get_hint(json_t *root, const char *hint_type, const char *hint);
int get_hint_int(json_t *root, const char *hint_type, const char *hint, int def);
double get_hint_double(json_t *root, const char *hint_type, const char *hint, double def);
char *get_hint_string(json_t *root, const char *hint_type, const char *hint, char *def);
#endif // HINT_H
	
