# CSC 369 Fall 2010 - Excercise 1
#
# Zeeshan Qureshi
# zeeshan.qureshi@utoronto.ca
# $Id$

all: clean thread_ex

thread_ex: thread_ex.c
	gcc -Wall -pedantic -g -lpthread --std=c99 -o $@ $^

clean:
	rm -f thread_ex
