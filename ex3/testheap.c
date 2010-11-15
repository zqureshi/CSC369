#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

#include "heap.h"

void getmem(void);

static void
h_test(unsigned ntest) {
    unsigned u, ux, ul;

    h_init(ntest);
    getmem();
    for (u = 0; u < ntest; u++)
        h_insert(random() % 10000);
	//dump_h();
    for (u = 0; u < ntest; u++) {
        h_remove();
        h_insert(random() % 10000);
    }
    ul = 0;
    for (u = 0; u < ntest; u++) {
        ux = h_remove();
        assert(ul <= ux);
        ul = ux;
    }
    getmem();
}

int main(int argc, char **argv) {
    //time_t start;

    if (argc < 2) {
        printf("Usage: %s <num_ops>\n", argv[0]);
        printf("       - num_ops is effectively the size of the heap\n");
        exit(-1);
    }
    unsigned long num_ops = atol(argv[1]);
    h_test(num_ops);

    //printf("Pagesize = %d\n", PAGESIZE);
    //printf("Datasize = %d\n", sizeof(struct data));
    //printf("Tree size = %lu\n", num_ops);
    //printf("System page size = %ld\n", sysconf(_SC_PAGESIZE));
    return 0;
}
