#include "hint.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <jansson.h>

#include "tonic.h"

json_t *get_hint(json_t *root, const char *hint_type, const char *hint)
{
	size_t category_len = strlen(hint_type) + strlen("_hints") + 1;
	char *category = malloc(category_len);
	snprintf(category, category_len, "%s_hints", hint_type);

	json_t *ret = json_object_get(json_object_get(root, category), hint);
	free(category);
	return ret;
}

int get_hint_int(json_t *root, const char *hint_type, const char *hint, int def)
{
	json_t *r = get_hint(root, hint_type, hint);
	int ret;
	if (!r) {
		ret = def;
	}
	else {
		ret = json_integer_value(r);
	}
	return ret;
}

double get_hint_double(json_t *root, const char *hint_type, const char *hint, double def)
{
	json_t *r = get_hint(root, hint_type, hint);
	double ret;
	if (!r) {
		ret = def;
	} else {
		ret = json_real_value(r);
	}
	return ret;
}

char *get_hint_string(json_t *root, const char *hint_type, const char *hint, char *def)
{
	json_t *r = get_hint(root, hint_type, hint);
	char *ret;
	if (!r) {
		ret = strdup(def);
	} else {
		ret = strdup(json_string_value(r));
	}
	return ret;
}
