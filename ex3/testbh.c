#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>

#include "bheap.h"

void getmem(void);

static void
bh_test(unsigned psz, unsigned ntest) {
    unsigned u, ux, ul;

    bh_init(psz, ntest);
    getmem();
    for (u = 0; u < ntest; u++)
        bh_insert(random() % 10000);
	//dump_bh();
    for (u = 0; u < ntest; u++) {
        bh_remove();
        bh_insert(random() % 10000);
    }
    ul = 0;
    for (u = 0; u < ntest; u++) {
        ux = bh_remove();
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
    bh_test(PAGESIZE, num_ops);

    //printf("Pagesize = %d\n", PAGESIZE);
    //printf("Datasize = %d\n", sizeof(struct data));
    //printf("Tree size = %lu\n", num_ops);
    //printf("System page size = %ld\n", sysconf(_SC_PAGESIZE));
    return 0;
}
