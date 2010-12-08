
f_test.o: \
 f_test.c \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/sys/stat.h \
 $(OSTREE)/include/kern/stat.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/err.h \
 f_hdr.h
f_read.o: \
 f_read.c \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/err.h \
 f_hdr.h
f_write.o: \
 f_write.c \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/err.h \
 f_hdr.h

