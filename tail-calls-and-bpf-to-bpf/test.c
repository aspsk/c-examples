#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>
#include <err.h>

#include <bpf/bpf.h>

#include "test.skel.h"

void sig_int(int signo)
{
	exit(0);
}

static void gimme_worker_fds(int *fd1, int *fd2)
{
	struct test_bpf *skel;
        struct bpf_link *link;
        int ret;

	skel = test_bpf__open();
	if (!skel)
		err(1, "%s: test_bpf__open", __func__);

	bpf_program__set_autoload(skel->progs.fentry1, false);
	bpf_program__set_autoload(skel->progs.fexit1, false);
	bpf_program__set_autoload(skel->progs.fentry2, false);
	bpf_program__set_autoload(skel->progs.fexit2, false);
	bpf_program__set_autoload(skel->progs.freplace, false);

	if (test_bpf__load(skel))
		err(1, "%s: test_bpf__open_load", __func__);

	*fd1 = bpf_program__fd(skel->progs.worker);
	*fd2 = bpf_program__fd(skel->progs.worker2);

        int map_fd = bpf_map__fd(skel->maps.jump_map);
        if (map_fd < 0)
                err(1, "%s: bpf_map__fd", __func__);

	int key = 0;
	ret = bpf_map_update_elem(map_fd, &key, fd2, BPF_ANY);
        if (ret)
                err(1, "%s: bpf_map_update_elem", __func__);

	link = bpf_program__attach(skel->progs.worker);
	ret = libbpf_get_error(link); 
	if (ret)
		err(1, "%s: bpf_program__attach", __func__);
}

static void trace(int fd1, const char *name1, int fd2, const char *name2)
{
	struct test_bpf *skel;

	skel = test_bpf__open();
	if (!skel)
		err(1, "%s: test_bpf__open", __func__);

	bpf_program__set_autoload(skel->progs.worker, false);
	bpf_program__set_autoload(skel->progs.worker2, false);

	bpf_program__set_expected_attach_type(skel->progs.fentry1, BPF_TRACE_FENTRY);
	bpf_program__set_attach_target(skel->progs.fentry1, fd1, name1);

	bpf_program__set_expected_attach_type(skel->progs.fexit1, BPF_TRACE_FEXIT);
	bpf_program__set_attach_target(skel->progs.fexit1, fd1, name1);

	bpf_program__set_expected_attach_type(skel->progs.fentry2, BPF_TRACE_FENTRY);
	bpf_program__set_attach_target(skel->progs.fentry2, fd2, name2);

	bpf_program__set_expected_attach_type(skel->progs.fexit2, BPF_TRACE_FEXIT);
	bpf_program__set_attach_target(skel->progs.fexit2, fd2, name2);

	bpf_program__set_attach_target(skel->progs.freplace, fd2, "woohoo");

	if (test_bpf__load(skel))
		err(1, "%s: test_bpf__load", __func__);

	if (test_bpf__attach(skel))
		err(1, "%s: test_bpf__attach", __func__);
}

int main(int argc, char **argv)
{
	int worker1_fd, worker2_fd;

	signal(SIGINT, sig_int);

	gimme_worker_fds(&worker1_fd, &worker2_fd);
	trace(worker1_fd, "worker", worker2_fd, "worker2");

	for ( ;; ) {
		syscall(SYS_getpgid);
		sleep(10);
	}
}
