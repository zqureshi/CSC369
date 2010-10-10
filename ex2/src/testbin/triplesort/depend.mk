
triplesort.o: \
 triplesort.c \
 triple.h
triple.o: \
 triple.c \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/err.h \
 triple.h

