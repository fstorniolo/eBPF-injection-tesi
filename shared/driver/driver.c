/*
 * Device driver for extensible paravirtualization QEMU device
 * 2022 Filippo Storniolo
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

#include <linux/processor.h>
#include <linux/sched.h>
#include <asm/uaccess.h> /* put_user */
#include <asm/io.h>
#include <linux/cdev.h> /* cdev_ */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kprobes.h>
#include <linux/bpf.h>
#include <linux/filter.h>

/* For socket etc */
#include <linux/net.h>
#include <linux/inet.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/file.h>
#include <linux/socket.h>

#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#include <asm/siginfo.h>    //siginfo
#include <linux/rcupdate.h> //rcu_read_lock
#include <linux/sched.h>    //find_task_by_pid_type

#include <linux/wait.h>		//linux wait_queue

#include "bpf_injection_msg.h"
#include "driver.h"

#include <linux/mmzone.h>
#include <linux/types.h>
#include <linux/mm_types.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/atomic.h>
#include <linux/slab.h>

#include <linux/types.h>
#include <linux/moduleparam.h>


#define NEWDEV_REG_PCI_BAR      0
#define NEWDEV_BUF_PCI_BAR      1
#define NEWDEV_PROG_PCI_BAR     2

#define DEV_NAME "newdev"
#define NEWDEV_DEVICE_ID 0x11ea
#define QEMU_VENDOR_ID 0x1234

#define NEWDEV_REG_STATUS_IRQ   0	//read
#define NEWDEV_REG_LOWER_IRQ    4	//write
#define NEWDEV_REG_RAISE_IRQ    8	//write (unused in this driver)
#define NEWDEV_REG_DOORBELL		8
#define NEWDEV_REG_SETAFFINITY	12
#define NEWDEV_BUF 				16
#define HEADER_OFFSET			4

#define MIGRATION_BUFFER_SIZE 	4 *1024*1024 /*4MiB*/

MODULE_LICENSE("GPL");

static struct pci_device_id pci_ids[] = {
	{ PCI_DEVICE(QEMU_VENDOR_ID, NEWDEV_DEVICE_ID), },
	{ 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

static int flag;
static int pci_irq;
static int major = 1111;
static struct pci_dev *pdev;
static void __iomem *bufmmio;
static	u8* __iomem progmmio;
unsigned long* migration_buffer;

void* ctx;


struct bpf_prog *bpf_injected_prog = NULL;
struct work_struct program_injection_work;

static DECLARE_WAIT_QUEUE_HEAD(wq);		//wait queue static declaration

static ssize_t mywrite(struct bpf_injection_msg_t mymsg, int mmio_offset)
{
	ssize_t ret = 0;
	int off = 0;

	// pr_info("WRITE SIZE=%ld, OFF=%lld", len, *off);
	iowrite32(*((u32*)&mymsg.header), bufmmio + mmio_offset);

	if(!(mymsg.header.payload_len % 4)){
		while(off < mymsg.header.payload_len){
			// pr_info("MY WRITE VAL: %d \n", kbuf);
			// pr_info("my write on off: %d \n",off);
			iowrite32(*((u32*)(mymsg.payload + off)), bufmmio + mmio_offset + off + 4);
			off += 4;
			ret += 4;
		}
	}

	else{
		pr_info("write NOT ALIGNED \n");
	}
	return ret;
}

static void write_guest_free_pages(void)
{
	// GET GUEST FREE PAGES
	int count = 0;
	u32 high_addr, low_addr, order;
	struct bpf_injection_msg_header header;
	struct zone* zone;
	// struct pglist_data* pgdat;
	struct free_area* free_area;
	struct page* page;
	unsigned long flags[2];
	unsigned long phys_addr;
	unsigned seq;
	unsigned long spanned_pages, start_pfn, present_pages;
	unsigned long managed_pages;
	struct list_head* tmp;
	int j;
	unsigned long free_area_pages;
	struct zone* zones[2];
	int k = 0;

	for_each_populated_zone(zone){
		printk(KERN_INFO "Populated Zone Name %s \n",zone->name);
		zones[k] = zone;
		k++;
	}

	k = 0;

	for_each_populated_zone(zone){
		printk(KERN_INFO "Populated Zone Name %s \n",zone->name);
		zones[k] = zone;
		spin_lock_irqsave(&zones[k]->lock, flags[k]);
		k++;
	}
	 


	for_each_zone(zone){
		if(zone == NULL){
			printk(KERN_INFO "NULL Pointer to zone");
			continue;
		}



		printk(KERN_INFO "Zone Name %s \n",zone->name);
		do {
			seq = zone_span_seqbegin(zone);
			start_pfn = zone->zone_start_pfn;
			spanned_pages = zone->spanned_pages;

		} while (zone_span_seqretry(zone, seq));

		present_pages = zone->present_pages;
		managed_pages = atomic_long_read(&zone->managed_pages);

		pr_info("start_pfn: %lu spanned_pages: %lu present_pages: %lu managed_pages: %lu \n", start_pfn, spanned_pages, present_pages, managed_pages);


		if(!managed_pages){
			pr_info("managed_pages is 0 \n");
			continue;
		}

		// spin_lock_irqsave(&zone->lock, flags);
		pr_info("Lock acquired \n");
		for(order = 0; order < MAX_ORDER; order++){
			// Get access to free page list with order i
			free_area = &zone->free_area[order];
			pr_info("free_area order %d has %lu free pages \n", order, free_area->nr_free);
			free_area_pages = 0;

			for(j = 0; j != MIGRATE_TYPES; j++){
				// Iterate on migratetype

				list_for_each(tmp, &free_area->free_list[j]){
					page = list_first_entry_or_null(tmp, struct page, lru);
	
					if(page == NULL){
						pr_info("page NULL \n");
						continue;
					}
					free_area_pages++;
					phys_addr = PFN_PHYS(page_to_pfn(page));
					// pr_info("page_to_pfn: %lx PAGE_SHIFT: %d \n", page_to_pfn(page), PAGE_SHIFT);
					// pr_info("Physical Address: %lx \n", phys_addr);

					migration_buffer[count] = phys_addr; 
					count++;
					migration_buffer[count] = order;
					count++;		
				}
			}
			pr_info("read pages %lu of %lu \n", free_area_pages, free_area->nr_free);

		}
			pr_info("\n");
	}

	header.version = DEFAULT_VERSION;
	header.type = FIRST_ROUND_MIGRATION;
	header.payload_len = 3 * sizeof(u32);

	high_addr = (virt_to_phys(migration_buffer) & 0xffffffff00000000) >> 32;
	low_addr = (virt_to_phys(migration_buffer) & 0x00000000ffffffff);

	pr_info("buffer gva address: %p \n",(void*) migration_buffer);
	pr_info("buffer gpa address: %llx \n", virt_to_phys(migration_buffer));
	pr_info("buffer gpa high address: %x", high_addr);
	pr_info("buffer gpa low address: %x", low_addr);

	iowrite32(*(u32*)&header, bufmmio + NEWDEV_BUF);
	iowrite32(high_addr, bufmmio + NEWDEV_BUF + HEADER_OFFSET);
	iowrite32(low_addr, bufmmio + NEWDEV_BUF + 4 + HEADER_OFFSET);
	iowrite32(count/2, bufmmio + NEWDEV_BUF + 8 + HEADER_OFFSET);

	iowrite32(0,bufmmio + NEWDEV_REG_DOORBELL);

	for_each_populated_zone(zone){
		printk(KERN_INFO "Populated Zone Name %s \n",zone->name);
		k--;
		zones[k] = zone;
		spin_unlock_irqrestore(&zones[k]->lock, flags[k]);
	}

}


void setup_migration_phase_start(void)
{
	write_guest_free_pages();
}


void setup_migration_phase_ended(void)
{
	struct zone *zone;
	unsigned long spanned_pages, start_pfn, present_pages;
	unsigned long managed_pages;
	unsigned seq;

	pr_info("setup_migration_phase_ended FUNCTION \n");

	for_each_populated_zone(zone){
		printk(KERN_INFO "Zone Name %s \n",zone->name);
		do {
			seq = zone_span_seqbegin(zone);
			start_pfn = zone->zone_start_pfn;
			spanned_pages = zone->spanned_pages;

		} while (zone_span_seqretry(zone, seq));

		present_pages = zone->present_pages;
		managed_pages = atomic_long_read(&zone->managed_pages);

		pr_info("start_pfn: %lu spanned_pages: %lu present_pages: %lu managed_pages: %lu \n", start_pfn, spanned_pages, present_pages, managed_pages);
	}
}

BPF_CALL_0(bpf_hv_test_helper)
{
	pr_info("BPF TEST PASSED \n");

	return 0; /* Checksum already done or not needed. */
}

BPF_CALL_0(bpf_hv_memory_info_helper)
{
	pr_info("BPF MEMORY INFO HELPER PASSED \n");


	return 0; /* Checksum already done or not needed. */
}

BPF_CALL_0(bpf_hv_set_maximum_page_order_helper)
{
	pr_info("BPF TEST PASSED \n");

	return 0; /* Checksum already done or not needed. */
}

static const char *
progname_from_idx(unsigned int prog_idx)
{
	switch (prog_idx) {
	case BPFHV_PROG_TEST:
		return "test";
	case BPFHV_PROG_GET_MEMORY_INFO:
		return "get_memory_info";
	case BPFHV_PROG_SET_MAX_PAGE_ORDER:
		return "set_max_page_order";
	default:
		break;
	}

	return NULL;
}

static int
bpfhv_helper_calls_fixup(struct bpf_insn *insns,
			size_t insns_count)
{
	size_t i;

	pr_info("Inside bpfhv_helper_calls_fixup \n");


	for (i = 0; i < insns_count; i++, insns++) {
		u64 (*func)(u64 r1, u64 r2, u64 r3, u64 r4, u64 r5);
		pr_info	("code: %x dst_reg: %x src_reg: %x off: %x imm: %x \n", insns->code, insns->dst_reg, insns->src_reg, insns->off, insns->imm);


		if (!(insns->code == (BPF_JMP | BPF_CALL) &&
			insns->dst_reg == 0 && insns->src_reg == 0 &&
				insns->off == 0)) {
			/* This is not an instruction that calls to
			 * an helper function. */
			continue;
		}

		switch (insns->imm) {
		case BPFHV_FUNC_test_helper:
			// pr_info("HELPER FUNCTION FOUND \n");
			// pr_info	("code: %x dst_reg: %x src_reg: %x off: %x imm: %x \n", insns->code, insns->dst_reg, insns->src_reg, insns->off, insns->imm);
			func = bpf_hv_test_helper;
			break;
		case BPFHV_FUNC_get_memory_info_helper:
			func = bpf_hv_memory_info_helper;
			break;
		case BPFHV_FUNC_set_maximum_page_order_helper:
			func = bpf_hv_set_maximum_page_order_helper;
			break;
		default:
			return -EINVAL;
			break;
		}

		insns->imm = func - __bpf_call_base;
	}

	return 0;
}

static void
print_program(struct bpf_insn *insns, size_t insns_count)
{
	struct bpf_insn *insn;
	int i;

	for(i = 0; i < insns_count; i++){
		insn = insns + i;
		pr_info	("code: %x dst_reg: %x src_reg: %x off: %x imm: %x \n", insn->code, insn->dst_reg, insn->src_reg, insn->off, insn->imm);
	}

}
/* Taken from kernel/bpf/syscall.c:bpf_prog_load(). */
static struct bpf_prog *
bpf_my_prog_alloc(const char *progname,
		struct bpf_insn *insns, size_t insn_count)
{
	struct bpf_prog *prog;
	int ret;

	pr_info("bpf_prog_size: %d \n", bpf_prog_size(insn_count));
	pr_info("progname: %s \n", progname);

	
	prog = bpf_prog_alloc(bpf_prog_size(insn_count), GFP_USER);
	if (!prog) {
		return NULL;
	}

	pr_info("post bpf_prog_alloc \n");

	print_program(insns, insn_count);

	
	prog->len = insn_count;
	memcpy(prog->insnsi, insns, bpf_prog_insn_size(prog));
	atomic64_set(&prog->aux->refcnt, 1);
	prog->gpl_compatible = 1;
	prog->type = BPF_PROG_TYPE_UNSPEC;
	prog->aux->load_time = ktime_get_boottime_ns();

	strlcpy(prog->aux->name, "hv-", sizeof(prog->aux->name));
	strlcat(prog->aux->name, progname, sizeof(prog->aux->name));

	//Replacement for bpf_check().
	prog->aux->stack_depth = MAX_BPF_STACK;

	pr_info("pre bpf_prog_select_runtime \n");
	prog = bpf_prog_select_runtime(prog, &ret);
	if (ret < 0) {
		pr_info("bpf_prog_select_runtime() failed: %d\n", ret);
		bpf_prog_free(prog);
		return NULL;
	}

	return prog;
}

static int
bpf_programs_setup(void)
{
	struct bpf_insn *insns;
	int ret = -EIO;
	int i;
	uint32_t program_size;
	struct bpf_injection_msg_header *msg_header;
	uint32_t *progp;
	size_t j, jmax;
	uint32_t header;


	header = ioread32(bufmmio + NEWDEV_BUF);
	msg_header = (struct bpf_injection_msg_header*)&header;
	program_size = msg_header->payload_len / 8;

	// program size is expressed in number of instructions
	pr_info("program size: %d \n", program_size);
	insns = kmalloc(program_size * sizeof(struct bpf_insn),
			GFP_ATOMIC);
	if (!insns) {
		pr_info("Error on insns \n");
		return -ENOMEM;
	}
	
	// Deallocate previous eBPF programs and the associated contexts. 
	if(bpf_injected_prog == NULL)
		bpf_prog_free(bpf_injected_prog);


	jmax = (program_size * sizeof(struct bpf_insn)) / sizeof(*progp);
	progp = (uint32_t *)insns;
	for (j = 0; j < jmax; j++, progp++) {
		*progp = readl(bufmmio + NEWDEV_BUF + HEADER_OFFSET +
				j * sizeof(*progp));
		// pr_info("bpf_programs_setup instruction j: %d \n", *progp);
	}

	/* Fix the immediate field of call instructions to helper
		* functions, replacing the abstract identifiers with actual
		* offsets. */

	ret = bpfhv_helper_calls_fixup(insns, program_size);
	if (ret) {
		pr_info("Error in bpfhv_helper_calls_fixup \n");
		goto out;
	}

	// Allocate an eBPF program for 'insns'. 

	pr_info("progname_from_idx: %s \n", progname_from_idx(BPFHV_PROG_TEST));

	
	bpf_injected_prog = bpf_my_prog_alloc(progname_from_idx(BPFHV_PROG_TEST),
					insns, program_size);
	if (bpf_injected_prog == NULL) {
		pr_info("Error in bpfhv_prog_alloc \n");
		goto out;
	}
	

	ret = 0;
out:
	kfree(insns);

	return ret;
}

static void execute_work(struct work_struct *w)
{
	int ret;

	pr_info("execute_work \n");
	
	ret = bpf_programs_setup();
	if (ret) {
		pr_info("program setup failed \n");
	}

	BPF_PROG_RUN(bpf_injected_prog, ctx);
	pr_info("program execution \n");
	
}

static irqreturn_t irq_handler(int irq, void *dev)
{
	int devi;
	irqreturn_t ret;
	u32 irq_status;
	devi = *(int *)dev;
	if (devi == major) {
		irq_status = ioread32(bufmmio + NEWDEV_REG_STATUS_IRQ);

		//handle
		pr_info("interrupt irq = %d dev = %d irq_status = 0x%llx\n",
				irq, devi, (unsigned long long)irq_status);
		pr_info("me handling like a god?\n");

		switch(irq_status){

			case PROGRAM_INJECTION:
				pr_info("case PROGRAM_INJECTION irq handler\n");
				schedule_work(&program_injection_work);

				break;

			case FIRST_ROUND_MIGRATION_ENDED:
				pr_info("case FIRST_ROUND_MIGRATION_ENDED irq handler \n");
				setup_migration_phase_ended();
				break;

			case FIRST_ROUND_MIGRATION_START:
				pr_info("case FIRST_ROUND_MIGRATION_START irq handler \n");
				setup_migration_phase_start();
				break;				

			case 22:		//init irq_handler (old raw data)
				pr_info("handling irq 22 for INIT\n");
				//init_handler();
				pr_info("waking up interruptible process...\n");
				flag = 1;
				wake_up_interruptible(&wq);
				break;
		}

		/* Must do this ACK, or else the interrupts just keeps firing. */
		iowrite32(irq_status, bufmmio + NEWDEV_REG_LOWER_IRQ);
		ret = IRQ_HANDLED;
	} else {
		ret = IRQ_NONE;
	}
	return ret;
}

/**
 * Called just after insmod if the hardware device is connected,
 * not called otherwise.
 *
 * 0: all good
 * 1: failed
 */
static int pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	u8 val;

	pr_info("pci_probe\n");

	pdev = dev;
	if (pci_enable_device(dev) < 0) {
		dev_err(&(pdev->dev), "pci_enable_device\n");
		goto error;
	}
	pr_info("pci_probe: pci_enable_device \n");

	// if (pci_request_region(dev, NEWDEV_REG_PCI_BAR, "myregion0")) {
	// 	dev_err(&(pdev->dev), "pci_request_region0\n");
	// 	goto error;
	// }
	// io = pci_iomap(pdev, NEWDEV_REG_PCI_BAR, pci_resource_len(pdev, NEWDEV_REG_PCI_BAR));

	if (pci_request_region(dev, NEWDEV_BUF_PCI_BAR, "myregion1")) {
		dev_err(&(pdev->dev), "pci_request_region1\n");
		goto error;
	}
	pr_info("pci_probe: pci_request_region \n");

	bufmmio = pci_iomap(pdev, NEWDEV_BUF_PCI_BAR, pci_resource_len(pdev, NEWDEV_BUF_PCI_BAR));
	pr_info("pci_probe: pci_iomap bufmmio \n");

	progmmio = pci_iomap(pdev, NEWDEV_PROG_PCI_BAR, pci_resource_len(pdev, NEWDEV_PROG_PCI_BAR));
	pr_info("pci_probe: pci_iomap progmmio \n");

	/* IRQ setup. */
	pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &val);
	pr_info("pci_probe: pci_read_config_byte \n");

	pci_irq = val;
	if (request_irq(pci_irq, irq_handler, IRQF_SHARED, "pci_irq_handler0", &major) < 0) {
		pr_info("pci_probe: error in request_irq, irq: %d \n", pci_irq);
		dev_err(&(dev->dev), "request_irq\n");
		goto error;
	}
	pr_info("pci_probe: pci_irq %d \n", pci_irq);

	INIT_WORK(&program_injection_work, execute_work);

	flag = 0;

	migration_buffer = kmalloc(MIGRATION_BUFFER_SIZE, GFP_KERNEL);
	if(migration_buffer == NULL){
		pr_info("Error allocating migration_buffer \n");
	} else {
		pr_info("migration_buffer allocated SUCCESSFULLY \n");
	}
	pr_info("pci_probe COMPLETED SUCCESSFULLY\n");

	ctx = kmalloc(4, GFP_KERNEL);



	// write_guest_free_pages();

	return 0;
error:
	return 1;
}

static void pci_remove(struct pci_dev *pdev)
{
	pr_info("pci_remove\n");

	pr_info("pci_remove: freeing pci_irq %d \n", pci_irq);
	free_irq(pci_irq, &major);

	pci_free_irq_vectors(pdev);
	pr_info("pci_remove: pci_free_irq_vectors OK \n");

	iounmap(bufmmio);
	pr_info("pci_remove: iounmap bufmmio OK\n");

	iounmap(progmmio);
	pr_info("pci_remove: iounmap progmmio OK\n");


	pci_disable_device(pdev);
	pr_info("pci_remove: pci_disable_device OK\n");

	pci_release_region(pdev, NEWDEV_BUF_PCI_BAR);
	pr_info("pci_remove: pci_release_selected_regions OK\n");

	kfree(ctx);
	kfree(migration_buffer);


}

static struct pci_driver pci_driver = {
	.name     = DEV_NAME,
	.id_table = pci_ids,
	.probe    = pci_probe,
	.remove   = pci_remove,
};

struct bpf_injection_msg_t prepare_bpf_message(u32* path, int type, int size){
	struct bpf_injection_msg_t mymsg;

	mymsg.header.version = DEFAULT_VERSION;
	mymsg.header.type = PROGRAM_INJECTION;
	mymsg.header.payload_len = size;

	mymsg.payload = path;

  	return mymsg;
}

void print_bpf_injection_message(struct bpf_injection_msg_header myheader){
	pr_info("  Version:%u\n  Type:%u\n  Payload_len:%u\n", myheader.version, myheader.type, myheader.payload_len);
}

static int myinit(void)
{	
	pr_info("Init Driver pr_info\n");
	printk(KERN_INFO "Init Driver printk");

	if (pci_register_driver(&pci_driver) < 0) {
		return 1;
	}
	
	return 0;
}

static void myexit(void)
{
	pci_unregister_driver(&pci_driver);

}

module_init(myinit);
module_exit(myexit);