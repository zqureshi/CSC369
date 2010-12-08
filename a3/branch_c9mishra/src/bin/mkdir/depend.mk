
mkdir.o: \
 mkdir.c \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/sys/stat.h \
 $(OSTREE)/include/kern/stat.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/err.h \
 $(OSTREE)/include/stdarg.h

