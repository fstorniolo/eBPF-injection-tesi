#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>


int main(){

	printf("Fill Memory \n");
	unsigned long size = 2 * 1024ul * 1024ul * 1024ul;
	// size += size / 2;
	char* fill;
	// allocate 1GB of memory
	mlock(fill, size);
	fill = new char[size];
	printf("array created \n");

	// write in memory
	for(long i = 0; i < size; i++)
		fill[i] = ('a' + i);
	system("read -p 'Press Enter to continue...' var");
	return 0;
}
