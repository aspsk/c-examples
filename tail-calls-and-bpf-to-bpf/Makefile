CLANG = clang
LLVM_STRIP = llvm-strip

ifeq ($(KERNEL),)
$(error I can't do this without your help, please point me to a kernel with KERNEL=<PATH>)
endif

CFLAGS += -I$(KERNEL)/usr/include
CFLAGS += -I$(KERNEL)/tools/bpf/bpftool/bootstrap/libbpf/include

.phony: all
all: test

vmlinux.h:
	bpftool btf dump file /sys/kernel/btf/vmlinux format c > $@

%.bpf.o: %.bpf.c vmlinux.h
	$(CLANG) \
		-I. \
		-I$(KERNEL)/tools/include/uapi/ \
		-I$(KERNEL)/tools/bpf/bpftool/bootstrap/libbpf/include \
		-g -O2 -Wall -target bpf -c $< -o $@
	$(Q)$(LLVM_STRIP) -g $@

%.skel.h: %.bpf.o
	bpftool gen skeleton $< > $@

LIBBPFA = $(KERNEL)/tools/bpf/bpftool/bootstrap/libbpf/libbpf.a

test: test.c test.skel.h
	$(CC) $(CFLAGS) -static test.c ${LIBBPFA} -o test -lelf -lz

.phony: clean
clean:
	${RM} test test.bpf.o test.skel.h vmlinux.h *.dot
