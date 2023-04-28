#include <vmlinux.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_helpers.h>

#define __section(X) __attribute__((section(X), used))

int error = 0;

__section("fentry/FUNC")
int BPF_PROG(fentry1)
{
	bpf_printk("fentry1");
	return 0;
}

__section("fexit/FUNC")
int BPF_PROG(fexit1, int ret)
{
	bpf_printk("fexit1");
	return 0;
}

__section("fentry/FUNC")
int BPF_PROG(fentry2)
{
	bpf_printk("fentry2");
	return 0;
}

__section("fexit/FUNC")
int BPF_PROG(fexit2, int ret)
{
	bpf_printk("fexit2");
	return 0;
}

__section("freplace/FUNC")
int freplace(bool what)
{
	if (what)
		bpf_printk("bacon spam eggs");
	else
		bpf_printk("spam spam spam spam spam");

	return 0;
}

__attribute__((noinline)) int woohoo(bool what)
{
	if (what)
		bpf_printk("spam");
	else
		bpf_printk("eggs");
	return 0;
}

struct {
        __uint(type, BPF_MAP_TYPE_PROG_ARRAY);
        __type(key, __u32);
        __type(value, __u32);
        __uint(max_entries, 1);
} jump_map __section(".maps");

__section("kprobe") int worker2(void *ctx)
{
	woohoo(!ctx);
	bpf_printk("hello from worker 2");
	return 0;
}

__section("kprobe/__x64_sys_getpgid")
int worker(void *ctx)
{
	bpf_printk("hello from worker");
	bpf_tail_call_static(ctx, &jump_map, 0);
	bpf_printk("goodbye from worker");
	return 0;
}

char _license[] __section("license") = "GPL";
