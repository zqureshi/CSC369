#ifndef _LIB_H_
#define _LIB_H_

/*
 * Miscellaneous standard C functions for the kernel, and other widely used
 * kernel functions.
 */

#include <machine/setjmp.h>

/*
 * Tell GCC to check printf formats.
 */
#ifdef __GNUC__
#define __PF(a,b) __attribute__((__format__(__printf__, a, b)))
#else
#define __PF(a,b)
#endif

/*
 * If not GCC, disable __PRETTY_FUNCTION__
 */
#ifndef __GNUC__
#define __PRETTY_FUNCTION__ "<unknown function>"
#endif

/*
 * Standard assert macro.
 */
#define assert(x) { \
    if (!(x)) { \
            panic("Assertion failed: %s, at %s:%d (%s)\n", #x, \
                    __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    } \
  }

/*
 * Bit flags for DEBUG()
 */
#define DB_LOCORE      0x001
#define DB_SYSCALL     0x002
#define DB_INTERRUPT   0x004
#define DB_DEVICE      0x008
#define DB_THREADS     0x010
#define DB_VM          0x020
#define DB_EXEC        0x040
#define DB_VFS         0x080
#define DB_SFS         0x100
#define DB_NET         0x200
#define DB_NETFS       0x400
#define DB_KMALLOC     0x800
#define DB_TLB         0x1000

extern u_int32_t dbflags;

/*
 * DEBUG() is for conditionally printing debug messages to the console.
 *
 * The idea is that you put lots of lines of the form
 *
 *      DEBUG(DB_VM, "VM free pages: %u\n", free_pages);
 *
 * throughout the kernel; then you can toggle whether these messages
 * are printed or not at runtime by setting the value of dbflags with
 * the debugger.
 *
 * Unfortunately, as of this writing, there are only a very few such
 * messages actually present in the system yet. Feel free to add more.
 *
 * DEBUG is a varargs macro. The first form is the standard C99
 * way. The second is the gcc way, which, in light of C99, is obsolete -
 * - but the gcc we're using doesn't know the C99 syntax yet.
 */
#if 0
#define DEBUG(d, fmt, ...) ((dbflags & (d)) ? kprintf(fmt, __VA_ARGS__) : 0)
#else
#define DEBUG(d, fmt, args...) ((dbflags & (d)) ? kprintf(fmt, ##args) : 0)
#endif

/*
 * Random number generator, using the random device.
 */
#define RAND_MAX (randmax())
u_int32_t randmax(void);
u_int32_t random(void);

/*
 * Kernel heap memory allocation. Like malloc/free.
 * If out of memory, kmalloc returns NULL.
 */
void *kmalloc(size_t sz);
void kfree(void *ptr);
void kheap_printstats(void);

/*
 * C string functions. 
 *
 * kstrdup is like strdup, but calls kmalloc instead of malloc.
 * If out of memory, it returns NULL.
 */
size_t strlen(const char *);
int strcmp(const char *, const char *);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
char *kstrdup(const char *);
char *strchr(const char *, int);
char *strrchr(const char *, int);
char *strtok_r(char *buf, const char *seps, char **context);

void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void bzero(void *, size_t);
int atoi(const char *);

int snprintf(char *buf, size_t maxlen, const char *fmt, ...) __PF(3,4);

const char *strerror(int errcode);

/*
 * setjmp/longjmp functionality.
 */

int setjmp(jmp_buf jb);
void longjmp(jmp_buf jb, int retval);

/*
 * Low-level console access.
 */
void putch(int ch);
int getch(void);
void beep(void);

/*
 * Higher-level console output.
 *
 * kprintf is like printf, only in the kernel.
 * panic prepends the string "panic: " to the message printed, and then
 * resets the system.
 * kgets is like gets, only with a buffer size argument.
 *
 * kprintf_init sets up a lock for kprintf and should be called during boot
 * once malloc is available and before any additional threads are created.
 */
int kprintf(const char *fmt, ...) __PF(1,2);
void panic(const char *fmt, ...) __PF(1,2);

void kgets(char *buf, size_t maxbuflen);

void kprintf_bootstrap(void);

/*
 * Byte swap functions for the kernel.
 */
u_int16_t ntohs(u_int16_t);
u_int16_t htons(u_int16_t);
u_int32_t ntohl(u_int32_t);
u_int32_t htonl(u_int32_t);
u_int64_t ntohll(u_int64_t);
u_int64_t htonll(u_int64_t);

/*
 * copyin/copyout/copyinstr/copyoutstr are standard BSD kernel functions.
 *
 * copyin copies LEN bytes from a user-space address USERSRC to a
 * kernel-space address DEST.
 *
 * copyout copies LEN bytes from a kernel-space address SRC to a
 * user-space address USERDEST.
 *
 * copyinstr copies a null-terminated string of at most LEN bytes from
 * a user-space address USERSRC to a kernel-space address DEST, and 
 * returns the actual length of string found in GOT. DEST is always
 * null-terminated on success. LEN and GOT include the null terminator.
 *
 * copyoutstr copies a null-terminated string of at most LEN bytes from
 * a kernel-space address SRC to a user-space address USERDEST, and 
 * returns the actual length of string found in GOT. DEST is always
 * null-terminated on success. LEN and GOT include the null terminator.
 *
 * All of these functions return 0 on success, EFAULT if a memory
 * addressing error was encountered, or (for the string versions)
 * ENAMETOOLONG if the space available was insufficient.
 *
 * NOTE that the order of the arguments is the same as bcopy() or 
 * cp/mv, that is, source on the left, NOT the same as strcpy().
 *
 * These functions are machine-dependent.
 */
 
int copyin(const_userptr_t usersrc, void *dest, size_t len);
int copyout(const void *src, userptr_t userdest, size_t len);
int copyinstr(const_userptr_t usersrc, char *dest, size_t len, size_t *got);
int copyoutstr(const char *src, userptr_t userdest, size_t len, size_t *got);

/*
 * Simple timing hooks.
 *
 * Threads sleeping on lbolt are woken up once a second.
 *
 * clocksleep() suspends execution for the requested number of seconds,
 * like userlevel sleep(3). (Don't confuse it with thread_sleep.)
 */
extern int lbolt;
void clocksleep(int seconds);

/*
 * Other miscellaneous stuff
 */

#define DIVROUNDUP(a,b) (((a)+(b)-1)/(b))
#define ROUNDUP(a,b)    (DIVROUNDUP(a,b)*b)

#define TRUE 1
#define FALSE 0

#endif /* _LIB_H_ */
