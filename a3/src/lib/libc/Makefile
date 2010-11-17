#
# Makefile for OS/161 C standard library
#

include ../../defs.mk

# Printf implementation
SRCS+=__printf.c snprintf.c printf.c

# C files for standard string operations
SRCS+=atoi.c bzero.c \
      memcmp.c memcpy.c memmove.c memset.c \
      strcat.c strchr.c strcmp.c strcpy.c strlen.c strrchr.c \
      strtok.c strtok_r.c

# Standard I/O functions
SRCS+=__assert.c __puts.c err.c getchar.c putchar.c puts.c 

# Other stuff
SRCS+=abort.c errno.c exit.c getcwd.c random.c strerror.c system.c time.c

# Machine-dependent setjmp implementation
SRCS+=$(PLATFORM)-setjmp.S

# System call entry points
SRCS+=syscalls.S

# Name of the library.
LIB=c

# Let the templates do most of the work.
include ../../mk/lib.mk

#
# Generate syscall entry points from system call list.
#

CALLNO_H=../../kern/include/kern/callno.h

syscalls.S: $(CALLNO_H) callno-parse.sh syscalls-$(PLATFORM).S ../../defs.mk
	-rm -f syscalls.S syscalls.S.tmp
	echo '/* Automatically generated from syscalls-$(PLATFORM).S */' \
		> syscalls.S.tmp
	cat syscalls-$(PLATFORM).S >> syscalls.S.tmp
	./callno-parse.sh < $(CALLNO_H) >> syscalls.S.tmp
	mv -f syscalls.S.tmp syscalls.S

clean: cleanhere
cleanhere:
	rm -f syscalls.S

depend: predepend
predepend:
	$(MAKE) syscalls.S

# Have the machine-dependent stuff depend on defs.mk in case the platform
# is changed.

syscalls.o $(PLATFORM)-setjmp.o: ../../defs.mk
