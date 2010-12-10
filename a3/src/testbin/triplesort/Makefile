# Makefile for triplesort

SRCS=triplesort.c triple.c
PROG=triplesort
BINDIR=/testbin

include ../../defs.mk
include ../../mk/prog.mk

progdepend: triple.c triple.h

triple.c:
	ln -s ../triplehuge/triple.c .

triple.h:
	ln -s ../triplehuge/triple.h .

clean: cleanhere
cleanhere:
	rm -f triple.c triple.h
