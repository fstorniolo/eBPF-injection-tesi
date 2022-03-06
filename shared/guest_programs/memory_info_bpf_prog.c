#include "../driver/driver.h"

#ifndef __section
# define __section(NAME)                  \
   __attribute__((section(NAME), used))
#endif

static int BPFHV_FUNC(get_memory_info_helper);

__section("get_memory_info")
int memory_info_prog(void* ctx)
{
   get_memory_info_helper();
   return 0;
}
   