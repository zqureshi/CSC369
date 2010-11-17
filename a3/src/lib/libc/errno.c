#include <errno.h>

/*
 * Source file that declares the space for the global variable errno.
 *
 * We also declare the space for __argv, which is used by the err*
 * functions. But since it is set by crt0, it is always referenced in
 * every program; putting it here prevents gratuitously linking all
 * the err* and warn* functions (and thus printf) into every program.
 */

char **__argv;

int errno;
