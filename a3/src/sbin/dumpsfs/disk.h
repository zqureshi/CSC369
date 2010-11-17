void opendisk(const char *path);

u_int32_t diskblocksize(void);
u_int32_t diskblocks(void);

void diskwrite(const void *data, u_int32_t block);
void diskread(void *data, u_int32_t block);

void closedisk(void);
