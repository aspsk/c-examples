#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <err.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

static void usage(const char *progname)
{
    fprintf(stderr, "usage: %s [-s|-e] filename\n", progname);
    exit(1);
}

/*
 * Lock file either shared or exclusive and release it after pressing 'Enter'
 *
 * Usage: flock_example [-s|-e] filename
 *
 * Options:
 *   -e: exclusive lock [default]
 *   -s: shared lock
 */
int main(int argc, char *argv[])
{
    int fd;
	const char *path;
    int op = LOCK_EX;

    if (argc > 3)
        usage(argv[0]);
    if (argc == 3) {
        if (!strcmp(argv[1], "-s"))
            op = LOCK_SH;
        else if (strcmp(argv[1], "-e"))
            usage(argv[0]);
    }

	path = argv[argc-1];

    fd = open(path, O_CREAT|O_RDWR, 0660);
    if (fd < 0)
        err(1, "open: %s", path);

    if (flock(fd, op))
        err(1, "flock");

    printf("locked file %s %s, press 'Enter' to release\n",
            path, op == LOCK_EX ? "exclusively" : "shared");

    getchar();

    printf("unlocked file %s\n", path);

    return 0;
}
