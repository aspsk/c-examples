# c-examples

A set of short unrelated programs written in C.  (The actual goal of this repo
is to gather short C examples scattered throughout my computers and backups.)

## The list of programs

* `directory_size/`: use openat and fstatat to compute the size of a directory (the same as `du -sb`, but slightly faster
* `eventfd/`: a simple example illustrating how the `eventfd(2)` system call can be used to synchronize a parent process and its children
* `flock_example/`: a simple example illustrating the `flock(2)` system call
* `so_attach_filter_udp/`: a simple example illustrating how to attach classic BPF filters to sockets
* `seccomp_filter/`: a simple example illustrating how to use `libseccomp`
* `tail-calls-and-bpf-to-bpf/`: a minimalistic example on how to use `fentry/fexit`, `freplace`, and tail calls in BPF
* `mapinspector/`: a minimalistic example on how to use BPF Map Iterators (by listing some properties of `hash_of_maps` unavailable via `bpftool`)
