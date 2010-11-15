void bh_init(unsigned psz, unsigned npages);
void bh_insert(unsigned val);
unsigned bh_remove(void);
void dump_bh(void);

// DATASIZE must be a power of 2 - sizeof int so that data will be a power
// of 2
#ifndef DATASIZE
#define DATASIZE 64
#endif

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

struct data {
    int key;
    char unused[DATASIZE -4];
};


