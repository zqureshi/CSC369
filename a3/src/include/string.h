#ifndef _STRING_H_
#define _STRING_H_

#include <sys/types.h>

/*
 * Standard C string functions.
 */
char *strcat(char *, const char *);
char *strcpy(char *, const char *);
char *strchr(const char *, int);
char *strrchr(const char *, int);
int strcmp(const char *, const char *);
size_t strlen(const char *);
char *strtok_r(char *, const char *, char **);
char *strtok(char *, const char *);

void *memset(void *, int c, size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);

/*
 * POSIX string functions.
 */
const char *strerror(int errcode);

/*
 * BSD string functions.
 */
void bzero(void *, size_t);


#endif /* _STRING_H_ */
