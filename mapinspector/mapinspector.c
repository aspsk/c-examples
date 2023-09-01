// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2023 Isovalent */
/* Copyright (c) 2023 Anton Protopopov */

#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#include "mapinspector.skel.h"

static void dump_info(int iter_fd)
{
        char buf[2048];
        ssize_t n;
        __u32 ret;

        n = read(iter_fd, buf, sizeof(buf)-1);
        if (n == 0)
		errx(0, "no info");
        if (n < 0)
		err(1, "read");
        buf[n] = '\0';

	printf("%s\n", buf);
}

static void inspect_map(int map_id)
{
        struct mapinspector_bpf *skel;
        struct bpf_link *link;
        __u32 n_elements;
        int iter_fd;
        int ret;

        skel = mapinspector_bpf__open();
        if (skel == NULL) err(1, "mapinspector__open");

        skel->bss->target_id = map_id;

        ret = mapinspector_bpf__load(skel);
        if (ret != 0) err(1, "mapinspector__load");

        link = bpf_program__attach_iter(skel->progs.dump_bpf_map, NULL);
        if(!link) err(1, "bpf_program__attach_iter");

        iter_fd = bpf_iter_create(bpf_link__fd(link));
        if (iter_fd < 0)
		err(1, "bpf_iter_create");

        dump_info(iter_fd);

        close(iter_fd);
        bpf_link__destroy(link);
        mapinspector_bpf__destroy(skel);
}

int main(int argc, char **argv)
{
	int map_id = atoi(argv[1]);
	warnx("trying map id=%d", map_id);
	inspect_map(map_id);
}
