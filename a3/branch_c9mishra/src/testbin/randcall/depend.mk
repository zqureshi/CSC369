
calls.o: \
 calls.c \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/sys/stat.h \
 $(OSTREE)/include/kern/stat.h \
 $(OSTREE)/include/assert.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/errno.h \
 $(OSTREE)/include/kern/errno.h \
 $(OSTREE)/include/err.h \
 extern.h
main.o: \
 main.c \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/string.h \
 extern.h

