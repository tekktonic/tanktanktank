#include "tonic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
char *strdup(const char *input)
{
	size_t ret_len = strlen(input+1);
	char *ret = malloc(ret_len+1);
	snprintf(ret, ret_len, "%s", input);
	ret[ret_len] = '\0';
	return ret;
}
*/

double step(double current, double towards, double increment)
{
    if (current < towards) {
        if (current+increment <= towards)
            return current+increment;
        goto tooclose;
    }
    if (current > towards) {
        if (current+increment >= towards)
            return current+increment;
        goto tooclose;
    }

tooclose:
    return towards;
}
