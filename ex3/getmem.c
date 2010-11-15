#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>


/* Collect current memory stats about the running process that calls getmem 
* Works by reading /proc/<pid>/stat
* Fields that we care about are:
* 10: minflt, 12: majflt, 14: utime, 15: stime, 23: vsize, 24 rss 
*/
void getmem() {
    pid_t pid = getpid();
    char path[128];
    sprintf(path, "/proc/%u/stat", pid);

    FILE *fp = fopen(path, "r");
    char line[256];
    if(fgets(line, 256, fp) == NULL) {
        perror("fgets");
    }

    unsigned long minflt, majflt, utime, stime, vsize;
    long rss;

    int i = 0;
    char *prev = line;
    char *next;
    while((next = strchr(prev, ' ')) != NULL) {
        i++;
        *next = '\0';
        if(i == 10) {
            minflt = strtol(prev, NULL, 0);
        } else if(i == 12) {
            majflt = strtol(prev, NULL, 0);
        } else if(i == 14) {
            utime = strtol(prev, NULL, 0);
        } else if(i == 15) {
            stime = strtol(prev, NULL, 0);
        } else if(i == 23) {
            vsize = strtol(prev, NULL, 0);
        } else if(i == 24) {
            rss = strtol(prev, NULL, 0);
        } else if(i > 24) {
            break;
        }


        next++;
        prev = next;
    }

    printf("%ld %ld %ld %ld %ld %ld\n", minflt, majflt, utime, stime, vsize, rss);

}
/* Main function just to test getmem */
/*
int main() {
sleep(10);
getmem();
return 0;
}*/
