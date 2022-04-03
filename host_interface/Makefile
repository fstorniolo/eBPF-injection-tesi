default: all
all: injectProgram.o memoryInfoInjectProgram.o setMaximumPageOrderInjectProgram.o enableOptimizationMigration.o disableOptimizationMigration.o

injectProgram.o:
	gcc injectProgram.c -l elf -o injectProgram.o

memoryInfoInjectProgram.o:
	gcc memoryInfoInjectProgram.c -l elf -o memoryInfoInjectProgram.o

setMaximumPageOrderInjectProgram.o:
	gcc setMaximumPageOrderInjectProgram.c -l elf -o setMaximumPageOrderInjectProgram.o

enableOptimizationMigration.o:
	gcc enableOptimizationMigration.c -l elf -o enableOptimizationMigration.o

disableOptimizationMigration.o:
	gcc disableOptimizationMigration.c -l elf -o disableOptimizationMigration.o

clean:
	-rm -f injectProgram.o memoryInfoInjectProgram.o setMaximumPageOrderInjectProgram.o enableOptimizationMigration.o disableOptimizationMigration.o
