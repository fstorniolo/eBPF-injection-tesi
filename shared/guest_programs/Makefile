#all: $(PROGS)

# proxy/%.o: proxy/%.c $(BEHDRS)
#	$(CC) -O2 -g -Wall -Werror -Wno-address-of-packed-member -I @SRCDIR@/include $(DEFS) -I @SRCDIR@/sched16 -c $< -o $@

test_bpf_prog.o: test_bpf_prog.c ../driver/driver.h
	clang -O2 -Wall -I @SRCDIR@../driver -target bpf -c $< -o $@
clean:
	-rm -rf test_bpf_prog.o
