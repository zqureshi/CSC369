#ifndef _KERN_TYPES_H_
#define _KERN_TYPES_H_

/*
 * Machine-independent types visible to user level.
 *
 * In many real systems, off_t and time_t are int64_t.
 */

typedef int32_t off_t;   /* Offset within file */
typedef int32_t pid_t;   /* Process ID */
typedef int32_t time_t;  /* Time in seconds */

#endif /* _KERN_TYPES_H_ */
