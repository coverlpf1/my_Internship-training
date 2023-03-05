#include </home/lpf/starnet/makefile-project/include/net/net.h>
#include </home/lpf/starnet/makefile-project/include/db/db.h>
#include <stdio.h>

int
ui()
{
  net();
  db();
  printf("ui\n");
  return 0;
}
