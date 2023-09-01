// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2023 Isovalent */
/* Copyright (c) 2023 Anton Protopopov */

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

__u32 target_id;

static __always_inline const char *bpf_map_type_s(const enum bpf_map_type map_type)
{
	switch(map_type) {
#define strcase(x) case x: return #x
	strcase(BPF_MAP_TYPE_UNSPEC);
	strcase(BPF_MAP_TYPE_HASH);
	strcase(BPF_MAP_TYPE_ARRAY);
	strcase(BPF_MAP_TYPE_PROG_ARRAY);
	strcase(BPF_MAP_TYPE_PERF_EVENT_ARRAY);
	strcase(BPF_MAP_TYPE_PERCPU_HASH);
	strcase(BPF_MAP_TYPE_PERCPU_ARRAY);
	strcase(BPF_MAP_TYPE_STACK_TRACE);
	strcase(BPF_MAP_TYPE_CGROUP_ARRAY);
	strcase(BPF_MAP_TYPE_LRU_HASH);
	strcase(BPF_MAP_TYPE_LRU_PERCPU_HASH);
	strcase(BPF_MAP_TYPE_LPM_TRIE);
	strcase(BPF_MAP_TYPE_ARRAY_OF_MAPS);
	strcase(BPF_MAP_TYPE_HASH_OF_MAPS);
	strcase(BPF_MAP_TYPE_DEVMAP);
	strcase(BPF_MAP_TYPE_SOCKMAP);
	strcase(BPF_MAP_TYPE_CPUMAP);
	strcase(BPF_MAP_TYPE_XSKMAP);
	strcase(BPF_MAP_TYPE_SOCKHASH);
	strcase(BPF_MAP_TYPE_CGROUP_STORAGE);
	strcase(BPF_MAP_TYPE_REUSEPORT_SOCKARRAY);
	strcase(BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE);
	strcase(BPF_MAP_TYPE_QUEUE);
	strcase(BPF_MAP_TYPE_STACK);
	strcase(BPF_MAP_TYPE_SK_STORAGE);
	strcase(BPF_MAP_TYPE_DEVMAP_HASH);
	strcase(BPF_MAP_TYPE_STRUCT_OPS);
	strcase(BPF_MAP_TYPE_RINGBUF);
	strcase(BPF_MAP_TYPE_INODE_STORAGE);
	strcase(BPF_MAP_TYPE_TASK_STORAGE);
	strcase(BPF_MAP_TYPE_BLOOM_FILTER);
	strcase(BPF_MAP_TYPE_USER_RINGBUF);
	strcase(BPF_MAP_TYPE_CGRP_STORAGE);
	}
}

SEC("iter/bpf_map")
int dump_bpf_map(struct bpf_iter__bpf_map *ctx)
{
        struct seq_file *seq = ctx->meta->seq;
        struct bpf_map *map = ctx->map;
	struct bpf_map *meta;

        if (map && map->id == target_id) {
		if (map->map_type != BPF_MAP_TYPE_HASH_OF_MAPS)
			return 0;

		meta = map->inner_map_meta;
                BPF_SEQ_PRINTF(seq, "inner_map->map_type=%s\n",   bpf_map_type_s(meta->map_type));
                BPF_SEQ_PRINTF(seq, "inner_map->key_size=%u\n",   meta->key_size);
                BPF_SEQ_PRINTF(seq, "inner_map->value_size=%u\n", meta->value_size);
                BPF_SEQ_PRINTF(seq, "inner_map->map_flags=%u",    meta->map_flags);
	}

        return 0;
}

char _license[] SEC("license") = "GPL";
