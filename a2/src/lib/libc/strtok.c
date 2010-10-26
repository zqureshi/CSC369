#include <string.h>

static char *__strtok_context;

char *
strtok(char *str, const char *seps)
{
	return strtok_r(str, seps, &__strtok_context);
}
