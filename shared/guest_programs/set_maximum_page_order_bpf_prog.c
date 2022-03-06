#include "../driver/driver.h"

#ifndef __section
# define __section(NAME)                  \
   __attribute__((section(NAME), used))
#endif

static int BPFHV_FUNC(set_maximum_page_order_helper);

__section("set_max_page_order")
int maximum_page_order_prog(void* ctx)
{
   set_maximum_page_order_helper();
   return 0;
}
   