#ifndef __DRIVER_H__
#define __DRIVER_H__
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * When compiling user-space code include <stdint.h>,
 * when compiling kernel-space code include <linux/types.h>
 */
#ifdef __KERNEL__
#include <linux/types.h>
#else  /* !__KERNEL__ */
#include <stdint.h>
#endif /* !__KERNEL__ */



/* Numbers for the helper calls used by bpfhv programs. */
#define BPFHV_HELPER_MAGIC	0x4b8f0000
enum bpfhv_helper_id {
	BPFHV_FUNC_test_helper = BPFHV_HELPER_MAGIC
};

#ifndef BPFHV_FUNC
#define BPFHV_FUNC(NAME, ...)              \
	(*NAME)(__VA_ARGS__) = (void *)BPFHV_FUNC_##NAME
#endif

/* Select the eBPF program to be read from the device. A guest can write to
 * the select register, and then read the program size (BPFHV_REG_PROG_SIZE)
 * and the actual eBPF code. The program size is expressed as a number of
 * eBPF instructions (with each instruction being 8 bytes wide). */
enum {
	BPFHV_PROG_NONE = 0,
	BPFHV_PROG_TEST,
	BPFHV_PROG_MAX,
};


#endif