# Makefile for dumpsfs

SRCS=dumpsfs.c disk.c support.c
PROG=dumpsfs
BINDIR=/sbin

include ../../defs.mk
include ../../mk/prog.mk
include ../../mk/hostprog.mk

all depend progdepend hostdepend: disk.c disk.h support.c support.h

disk.c:
	ln -s ../mksfs/disk.c .

disk.h:
	ln -s ../mksfs/disk.h .

support.c:
	ln -s ../mksfs/support.c .

support.h:
	ln -s ../mksfs/support.h .

