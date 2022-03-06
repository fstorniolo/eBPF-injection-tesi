default: all
all: injectProgram.o memoryInfoInjectProgram.o setMaximumPageOrderInjectProgram.o

injectProgram.o:
	gcc injectProgram.c -l elf -o injectProgram.o

memoryInfoInjectProgram.o:
	gcc memoryInfoInjectProgram.c -l elf -o memoryInfoInjectProgram.o

setMaximumPageOrderInjectProgram.o:
	gcc setMaximumPageOrderInjectProgram.c -l elf -o setMaximumPageOrderInjectProgram.o

clean:
	-rm -f injectProgram.o memoryInfoInjectProgram.o setMaximumPageOrderInjectProgram.o
