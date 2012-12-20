#include <stdlib.h>
#include <bitpack.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

int main(){
  printf("%" PRIu64 "\n", (uint64_t)2 << 59);
  return 0;
}
