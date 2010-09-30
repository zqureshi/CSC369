#ifndef _BITMAP_H_
#define _BITMAP_H_

/*
 * Fixed-size array of bits. (Intended for storage management.)
 *
 * Functions:
 *     bitmap_create  - allocate a new bitmap object.
 *                      Returns NULL on error.
 *     bitmap_getdata - return pointer to raw bit data (for I/O).
 *     bitmap_alloc   - locate a cleared bit, set it, and return its index.
 *     bitmap_mark    - set a clear bit by its index.
 *     bitmap_unmark  - clear a set bit by its index.
 *     bitmap_isset   - return whether a particular bit is set or not.
 *     bitmap_destroy - destroy bitmap.
 */

/*
 * It would be a lot more efficient on most platforms to use u_int32_t
 * or unsigned long as the base type for holding bits. But we don't,
 * because if one uses any data type more than a single byte wide,
 * bitmap data saved on disk becomes endian-dependent, which is a
 * severe nuisance.
 */


#define BITS_PER_WORD   (CHAR_BIT)
#define WORD_TYPE       unsigned char
#define WORD_ALLBITS    (0xff)

struct bitmap {
        u_int32_t nbits;
        WORD_TYPE *v;
};

struct bitmap *bitmap_create(u_int32_t nbits);
void          *bitmap_getdata(struct bitmap *);
int            bitmap_alloc(struct bitmap *, u_int32_t *index);
void           bitmap_mark(struct bitmap *, u_int32_t index);
void           bitmap_unmark(struct bitmap *, u_int32_t index);
int	       bitmap_isset(struct bitmap *, u_int32_t index);
void           bitmap_destroy(struct bitmap *);

#endif /* _BITMAP_H_ */
