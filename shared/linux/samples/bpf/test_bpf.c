#include <uapi/linux/bpf.h>
#include "bpf_helpers.h"

int bpf_prog(void *ctx) {
    char buf[] = "Hello World!\n";
    bpf_trace_printk("XXX", 3);
    return 0;
}
