#ifndef _ASSERT_H_
#define _ASSERT_H_

/*
 * Required by ANSI
 */
#undef assert

/*
 * Function to call when an assert fails.
 */
void __bad_assert(const char *file, int line, const char *msg);

/*
 * Asserts are only "on" if NDEBUG isn't set. (This is standard C.)
 *
 * If asserts are off, we retain the expression but ignore it by
 * casting it to void. If the expression has no side-effects, or is
 * constant, this should generate no code. On the other hand, if it
 * has side effects, the side effects are preserved. This way if you
 * write 
 *         assert(myfunc() == 0);
 *
 * to check that myfunc() didn't fail, turning off asserts will not
 * cause a bug by causing the call to evaporate. Note that this
 * behavior is not standard and should not be relied upon.
 */

#ifdef NDEBUG
#define assert(x) ((void)(x))
#else
#define assert(x) ((x) ? (void)0 : __bad_assert(__FILE__, __LINE__, #x))
#endif


#endif /* _ASSERT_H_ */
