#include "../driver/driver.h"

#ifndef __section
# define __section(NAME)                  \
   __attribute__((section(NAME), used))
#endif

static int BPFHV_FUNC(test_helper);

__section("test")
int test_prog(void* ctx)
{
   test_helper();
   return 0;
}
   