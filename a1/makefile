# CSC 269 Fall 2010 - Assignment 1
#
# $Id$

FLAGS = -Wall -g --pedantic --std=c99 -D '_GNU_SOURCE'
LIBS  = -lpthread -lm

all: clean simcache

simcache: rv.c cache.c simcache.c
	gcc ${FLAGS} ${LIBS} -o $@ $^

clean:
	rm -f simcache
