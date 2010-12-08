
__printf.o: \
 __printf.c \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/assert.h \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/stdarg.h
snprintf.o: \
 snprintf.c \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h
printf.o: \
 printf.c \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h
atoi.o: \
 atoi.c \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/string.h
bzero.o: \
 bzero.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
memcmp.o: \
 memcmp.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
memcpy.o: \
 memcpy.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
memmove.o: \
 memmove.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
memset.o: \
 memset.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strcat.o: \
 strcat.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strchr.o: \
 strchr.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strcmp.o: \
 strcmp.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strcpy.o: \
 strcpy.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strlen.o: \
 strlen.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strrchr.o: \
 strrchr.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strtok.o: \
 strtok.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strtok_r.o: \
 strtok_r.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
__assert.o: \
 __assert.c \
 $(OSTREE)/include/assert.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/string.h
__puts.o: \
 __puts.c \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h
err.o: \
 err.c \
 $(OSTREE)/include/err.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/errno.h \
 $(OSTREE)/include/kern/errno.h \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/stdlib.h
getchar.o: \
 getchar.c \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h
putchar.o: \
 putchar.c \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h
puts.o: \
 puts.c \
 $(OSTREE)/include/stdio.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/stdarg.h
abort.o: \
 abort.c \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h
errno.o: \
 errno.c \
 $(OSTREE)/include/errno.h \
 $(OSTREE)/include/kern/errno.h
exit.o: \
 exit.c \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h
getcwd.o: \
 getcwd.c \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/errno.h \
 $(OSTREE)/include/kern/errno.h
random.o: \
 random.c \
 $(OSTREE)/include/assert.h \
 $(OSTREE)/include/errno.h \
 $(OSTREE)/include/kern/errno.h \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h
strerror.o: \
 strerror.c \
 $(OSTREE)/include/string.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/kern/errmsg.h
system.o: \
 system.c \
 $(OSTREE)/include/errno.h \
 $(OSTREE)/include/kern/errno.h \
 $(OSTREE)/include/stdlib.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h \
 $(OSTREE)/include/string.h
time.o: \
 time.c \
 $(OSTREE)/include/unistd.h \
 $(OSTREE)/include/sys/types.h \
 $(OSTREE)/include/machine/types.h \
 $(OSTREE)/include/kern/types.h \
 $(OSTREE)/include/kern/unistd.h \
 $(OSTREE)/include/kern/ioctl.h
mips-setjmp.o: \
 mips-setjmp.S \
 $(OSTREE)/include/machine/asmdefs.h
syscalls.o: \
 syscalls.S \
 $(OSTREE)/include/kern/callno.h \
 $(OSTREE)/include/machine/asmdefs.h

