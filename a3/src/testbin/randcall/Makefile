# Makefile for badcall

SRCS=calls.c main.c
PROG=randcall
BINDIR=/testbin

include ../../defs.mk
include ../../mk/prog.mk

calls.c: gencalls.sh callspecs.txt
	./gencalls.sh callspecs.txt > calls.c

depend progdepend: predepend
predepend:
	$(MAKE) calls.c

clean: myclean
myclean:
	rm -f calls.c
