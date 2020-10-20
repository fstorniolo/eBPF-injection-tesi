/*
 * BPF program to monitor CPU affinity tuning
 * 2020 Giacomo Pellicci
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <uapi/linux/bpf.h>
#include <linux/version.h>
#include <linux/ptrace.h>


#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

#define _(P) ({typeof(P) val = 0; bpf_probe_read(&val, sizeof(val), &P); val;})
#define MAX_ENTRIES 64


// Using BPF_MAP_TYPE_ARRAY map type all array elements pre-allocated 
// and zero initialized at init time

struct bpf_map_def SEC("maps") values = {
	.type = BPF_MAP_TYPE_ARRAY,
	.key_size = sizeof(u32),
	.value_size = sizeof(u64),
	.max_entries = MAX_ENTRIES,
};

/*	System call prototype:	
		asmlinkage long sys_sched_setaffinity(pid_t pid, unsigned int len,
					unsigned long __user *user_mask_ptr);

	In-kernel function sched_setaffinity has the following prototype:
		sched_setaffinity(pid_t pid, const struct cpumask *new_mask);

	This is what we kprobe, not the system call.
*/


/* kprobe is NOT a stable ABI
 * kernel functions can be removed, renamed or completely change semantics.
 * Number of arguments and their positions can change, etc.
 * In such case this bpf+kprobe example will no longer be meaningful
*/
SEC("kprobe/sched_setaffinity")
int bpf_prog1(struct pt_regs *ctx){
	int ret;
	// int pid;
	u64 cpu_set;
	u64 *top;
	u32 index = 0;

	// char fmt[] = "cpu_set %lu\n";
	// bpf_trace_printk(fmt, sizeof(fmt), cpu_set);

	// Read from onst struct cpumask *new_mask (2nd parameter)
	
	// pid = (int)PT_REGS_PARM1(ctx);
	ret = bpf_probe_read(&cpu_set, 8, (void*)PT_REGS_PARM2(ctx));

	top = bpf_map_lookup_elem(&values, &index);	
	if (!top){		
		return 0;
	}
	if(*top == MAX_ENTRIES-1){
		return 0;
	}
	__sync_fetch_and_add(top, 1);
	index = *top;
	// bpf_trace_printk(fmt, sizeof(fmt), cpu_set);
	bpf_map_update_elem(&values, &index, &cpu_set, 0);
    return 0;    
}

char _license[] SEC("license") = "GPL";
u32 _version SEC("version") = LINUX_VERSION_CODE;		//Useful because kprobe is NOT a stable ABI. (wrong version fails to be loaded)
