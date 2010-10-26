#define TESTFILE "badcallfile"
#define TESTDIR  "badcalldir"
#define TESTLINK "badcalllink"

/* driver.c */
void report_survival(int rv, int error, const char *desc);
void report_test(int rv, int error, int right_error, const char *desc);
void report_test2(int rv, int error, int okerr1, int okerr2, const char *desc);
int open_testfile(const char *str);
int create_testfile(void);
int create_testdir(void);
int create_testlink(void);

/* common_buf.c */
void test_read_buf(void);
void test_write_buf(void);
void test_getdirentry_buf(void);
void test_getcwd_buf(void);
void test_readlink_buf(void);

/* common_fds.c */
void test_read_fd(void);
void test_write_fd(void);
void test_close_fd(void);
void test_ioctl_fd(void);
void test_lseek_fd(void);
void test_fsync_fd(void);
void test_ftruncate_fd(void);
void test_fstat_fd(void);
void test_getdirentry_fd(void);
void test_dup2_fd(void);

/* common_path.c */
void test_open_path(void);
void test_remove_path(void);
void test_rename_paths(void);
void test_link_paths(void);
void test_mkdir_path(void);
void test_rmdir_path(void);
void test_chdir_path(void);
void test_symlink_paths(void);
void test_readlink_path(void);
void test_stat_path(void);
void test_lstat_path(void);

/* bad_*.c */
void test_execv(void);
void test_waitpid(void);
void test_open(void);
void test_read(void);
void test_write(void);
void test_close(void);
void test_reboot(void);
void test_sbrk(void);
void test_ioctl(void);
void test_lseek(void);
void test_fsync(void);
void test_ftruncate(void);
void test_fstat(void);		/* in bad_stat.c */
void test_remove(void);
void test_rename(void);
void test_link(void);
void test_mkdir(void);
void test_rmdir(void);
void test_chdir(void);
void test_getdirentry(void);
void test_symlink(void);
void test_readlink(void);
void test_dup2(void);
void test_pipe(void);
void test_time(void);
void test_getcwd(void);
void test_stat(void);
void test_lstat(void);		/* in bad_stat.c */
