PAGESIZE = 4096
DATASIZE = 256
FLAGS = -DPAGESIZE=${PAGESIZE} -DDATASIZE=${DATASIZE}


all : testbh testheap

testheap : heap.o testheap.o getmem.o
	gcc ${FLAGS} -Wall -g -o testheap $^

testbh : bheap.o testbh.o getmem.o
	gcc ${FLAGS} -Wall -g -o testbh $^
	
%.o : %.c
	gcc ${FLAGS} -Wall -g -c $^
	
testheap.o : heap.h
heap.o : heap.h
testbh.o : bheap.h
bheap.o : bheap.h

clean : 
	rm *.o testbh testheap
