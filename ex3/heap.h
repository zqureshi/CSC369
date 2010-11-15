void h_init(unsigned npages);
void h_insert(unsigned val);
unsigned h_remove(void);
void dump_h(void);

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


