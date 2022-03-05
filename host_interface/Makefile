default: all

all: injectProgram.c bpf_injection_msg.h
	gcc injectProgram.c -l elf -o injectProgram
clean:
	-rm -f injectProgram
