/*
 * BPF message injection library
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <libelf.h>
#include <gelf.h>
#include <fcntl.h>
#include <string.h>



//cut

/*
*
*	Message structure used to exchange information between guest
*	and host during setup and execution phase of given eBPF programs.
*	Typical workflow is to have the host sending a message containing 
*	the eBPF program to be executed and then receive from guest a result
*	to be used in the specific scenario.
*
*/

/* type defines */
#define PROGRAM_INJECTION 						1
#define PROGRAM_INJECTION_RESULT 				2
#define PROGRAM_MEMORY_INFO 					3
#define PROGRAM_SET_MAXIMUM_ORDER				4
#define FIRST_ROUND_MIGRATION					20
#define ENABLE_MIGRATION_SETUP_OPTIMIZATION     10
#define DISABLE_MIGRATION_SETUP_OPTIMIZATION    11
/* version defines */
#define DEFAULT_VERSION 						1

// +----+---------+------+----------------+
// | 0  | version | type | payload length |
// +----+---------+------+----------------+
// | 32 |                                 |
// +----+             payload             |
// | 64 |                                 |
// +----+---------------------------------+

struct bpf_injection_msg_header;
struct bpf_injection_msg_t;
struct bpf_injection_msg_t prepare_bpf_injection_message(const char* path, const char* section_name, uint8_t header_type);
void print_bpf_injection_message(struct bpf_injection_msg_header myheader);

struct bpf_injection_msg_header {
	uint8_t version;		//version of the protocol
	uint8_t type;			//what kind of payload is carried
	uint16_t payload_len;	//payload length
};

struct bpf_injection_msg_t {
	struct bpf_injection_msg_header header;
	void* payload;
};

struct bpf_injection_msg_t prepare_bpf_injection_message(const char* path, const char* section_name, uint8_t header_type){
	struct bpf_injection_msg_t mymsg;
	int len;
	int fd;
	mymsg.header.version = DEFAULT_VERSION;
	mymsg.header.type = header_type;

    GElf_Ehdr ehdr;
    int ret = -1;
    Elf *elf;
    int i;

	fd = open(path, O_RDONLY, 0);
	if (fd < 0) {
        printf("Failed to open %s \n", path);

        // error_setg_errno(errp, errno, "Failed to open %s", path);
    }

    printf("path: %s aperto \n", path);

    if (elf_version(EV_CURRENT) == EV_NONE) {
        printf("ELF version mismatch \n");
        // return -1;
    }
    elf = elf_begin(fd, ELF_C_READ, NULL);
    if (!elf) {
        printf("Failed to initialize ELF library for %s", path);
        // return -1;
    }

    if (gelf_getehdr(elf, &ehdr) != &ehdr) {
        printf("Failed to get ELF header for %s", path);
        goto err;
    }

    for (i = 1; i < ehdr.e_shnum; i++) {
        Elf_Data *sdata;
        GElf_Shdr shdr;
        Elf_Scn *scn;
        char *shname;

        scn = elf_getscn(elf, i);
        if (!scn) {
            continue;
        }

        if (gelf_getshdr(scn, &shdr) != &shdr) {
            continue;
        }

        if (shdr.sh_type != SHT_PROGBITS) {
            continue;
        }

        shname = elf_strptr(elf, ehdr.e_shstrndx, shdr.sh_name);
        if (!shname || shdr.sh_size == 0) {
            continue;
        }

        sdata = elf_getdata(scn, NULL);
        if (!sdata || elf_getdata(scn, sdata) != NULL) {
            continue;
        }

        {
        	#if 0
            int j;
            printf("prima di strcmp \n");
            for (j = 0; j < /*ARRAY_SIZE(prog_names)*/ 1; j++) {
                if (!strcmp(shname, prog_names[j])) {
                    break;
                }
            }
            printf("dopo di strcmp \n");

            if (j >= /*ARRAY_SIZE(prog_names)*/ 1) {
                continue;
            }
            #endif
            
        	if(!strcmp(shname, section_name)){
        		printf("shname e section_name are equal \n");
        		printf("shname: %s section_name: %s \n", shname, section_name);
        	}

			mymsg.header.payload_len = sdata->d_size;	
			mymsg.payload = malloc(mymsg.header.payload_len);

			printf("Payload_len: %ld \n", sdata->d_size);
  

            // s->prog->insns = g_malloc(sdata->d_size);
            memcpy(mymsg.payload, sdata->d_buf, sdata->d_size);
            //s->prog->num_insns = sdata->d_size / BPF_INSN_SIZE; BPF_INSN_SIZE == 8
            printf("numinsns equivalent: %ld \n", sdata->d_size / 8);
        }
    }

    ret = 0;
    // pstrcpy(s->progsname, sizeof(s->progsname), progsname);
    // DBG("Loaded program: %s", s->progsname);
err:
    elf_end(elf);

  	return mymsg;
}

void print_bpf_injection_message(struct bpf_injection_msg_header myheader){
	printf("  Version:%u\n  Type:%u\n  Payload_len:%u\n", myheader.version, myheader.type, myheader.payload_len);
}
