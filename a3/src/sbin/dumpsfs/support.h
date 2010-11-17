
#ifdef __osf__
/* Digital Unix (aka Compaq Tru64) */
#define HAS_NO_SIZED_TYPES
#endif

#if defined(__sun__) && defined(__svr4__)
/* Solaris */
#define HAS_NO_SIZED_TYPES
#endif

/*
 * Some systems don't have u_int32_t, u_int16_t, or u_int8_t.
 * (Sometime this should be changed so it gets probed by the configure script.)
 */

#ifdef HAS_NO_SIZED_TYPES

#if defined(__alpha__) || defined(__alpha)
/* Alpha processor: lp64 */
typedef unsigned int u_int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;

#elif defined(__sparc__)
/* Sparc processor: 32-bit or lp64 */
typedef unsigned int u_int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;

#else
#error "HAS_NO_SIZED_TYPES defined and I don't know what the sizes should be"
#endif

#endif
