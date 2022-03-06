all: test_bpf_prog.o memory_info_bpf_prog.o set_maximum_page_order_bpf_prog.o

test_bpf_prog.o: test_bpf_prog.c ../driver/driver.h
	clang -O2 -Wall -I @SRCDIR@../driver -target bpf -c $< -o $@

memory_info_bpf_prog.o: memory_info_bpf_prog.c ../driver/driver.h
	clang -O2 -Wall -I @SRCDIR@../driver -target bpf -c $< -o $@

set_maximum_page_order_bpf_prog.o: set_maximum_page_order_bpf_prog.c ../driver/driver.h
	clang -O2 -Wall -I @SRCDIR@../driver -target bpf -c $< -o $@

clean:
	-rm -rf test_bpf_prog.o memory_info_bpf_prog.o set_maximum_page_order_bpf_prog.o
