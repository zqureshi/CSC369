#include <unistd.h>

int
main()
{
  execv("/testbin/getpidtest", NULL);
  return 0;
}
