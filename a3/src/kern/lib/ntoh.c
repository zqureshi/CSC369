/*
 * Byte-swap functions.
 *
 * *s are for "short" (16-bit)
 * *l are for "long" (32-bit)
 * *ll are for "long long" (64-bit)
 *
 * hton* convert from host byte order to network byte order.
 * ntoh* convert from network byte order to host byte order.
 *
 * Network byte order is big-endian.
 *
 * Note that right now the only platforms OS/161 runs on are
 * big-endian, so these functions are actually all empty.
 */

#include <types.h>
#include <lib.h>

#ifdef _LITTLE_ENDIAN

#define SWAPFUNCS(size, type)                               \
    type                                                    \
    ntoh##size(volatile type val)                           \
    {                                                       \
            volatile type rv;                               \
            volatile char *src, *dest;                      \
	    unsigned i;                                     \
	    src = (char *)&val;                             \
	    dest = (char *)&rv;                             \
	    for (i=0; i<sizeof(rv); i++) {                  \
		    dest[i] = src[sizeof(rv)-i-1];          \
	    }                                               \
	    return rv;                                      \
                                                            \
    }                                                       \
                                                            \
    type                                                    \
    hton##size(type val)                                    \
    {                                                       \
            return ntoh##size(val);                         \
    }
#endif

/*
 * Use a separate #if, so if the header file defining the symbols gets
 * omitted or messed up the build will fail instead of silently choosing
 * the wrong option.
 */
#ifdef _BIG_ENDIAN

#define SWAPFUNCS(size, type) \
   type ntoh##size(type val) { return val; } \
   type hton##size(type val) { return val; }

#endif

#ifndef SWAPFUNCS
#error "_BIG_ENDIAN/_LITTLE_ENDIAN not set"
#endif

SWAPFUNCS(s, u_int16_t)
SWAPFUNCS(l, u_int32_t)
SWAPFUNCS(ll, u_int64_t)
