#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <seccomp.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

static int sys_numbers[] = {
	__NR_mount,
	__NR_umount2,
	__NR_ptrace,
	__NR_kexec_load,
	__NR_open_by_handle_at,
	__NR_init_module,
	__NR_finit_module,
	__NR_delete_module,
	__NR_iopl,
	__NR_ioperm,
	__NR_swapon,
	__NR_swapoff,
	__NR_syslog,
	__NR_process_vm_readv,
	__NR_process_vm_writev,
	__NR_sysfs,
	__NR__sysctl,
	__NR_adjtimex,
	__NR_clock_adjtime,
	__NR_lookup_dcookie,
	__NR_perf_event_open,
	__NR_fanotify_init,
	__NR_kcmp,
	__NR_add_key,
	__NR_request_key,
	__NR_keyctl,
	__NR_uselib,
	__NR_acct,
	__NR_modify_ldt,
	__NR_pivot_root,
	__NR_io_setup,
	__NR_io_destroy,
	__NR_io_getevents,
	__NR_io_submit,
	__NR_io_cancel,
	__NR_remap_file_pages,
	__NR_mbind,
	__NR_get_mempolicy,
	__NR_set_mempolicy,
	__NR_migrate_pages,
	__NR_move_pages,
	__NR_vmsplice,
	__NR_perf_event_open,
};

int main(int argc, char **argv)
{
	scmp_filter_ctx ctx;
	size_t i;

	if (argc < 2) {
		fprintf(stderr, "usage: seccomp_lib <prog>\n");
		exit(1);
	}

	ctx = seccomp_init(SCMP_ACT_ALLOW);
	if (!ctx)
		err(1, "seccomp_init");

	for (i = 0; i < sizeof(sys_numbers)/sizeof(sys_numbers[0]); i++)
		seccomp_rule_add(ctx, SCMP_ACT_TRAP, sys_numbers[i], 0);

	seccomp_load(ctx);

	execvp(argv[1], &argv[1]);
	err(1, "execlp: %s", argv[1]);
}
