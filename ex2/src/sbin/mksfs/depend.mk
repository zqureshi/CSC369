
mksfs.o: \
 mksfs.c \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/assert.h \
 $(OSTREE)/include/limits.h \
 $(OSTREE)/include/kern/limits.h \
 $(OSTREE)/include/err.h \
 $(OSTREE)/include/stdarg.h \
 support.h \
 $(OSTREE)/include/kern/sfs.h \
 disk.h
disk.o: \
 disk.c \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/sys/stat.h \
 $(OSTREE)/include/kern/stat.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/assert.h \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/errno.h \
 $(OSTREE)/include/kern/errno.h \
 $(OSTREE)/include/fcntl.h \
 $(OSTREE)/include/err.h \
 $(OSTREE)/include/stdarg.h \
 support.h \
 disk.h
support.o: \
 support.c

