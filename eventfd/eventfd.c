#define _BSD_SOURCE		/* usleep */
#define _POSIX_SOURCE	/* rand   */

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/eventfd.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int wait_children(int fd, long long N)
{
    long long x;

    while (N > 0) {
        if (read(fd, &x, sizeof(x)) != sizeof(x)) {
            warn("read");
            return -1;
        }
        N -= x;
    }

    return 0;
}

void signal_parent(int fd)
{
    long long x = 1;

    if (write(fd, &x, sizeof(x)) != sizeof(x))
        warn("write");
}

int main()
{
    int status;
    int event_fd;
    int N_children = 10;
    pid_t pid[N_children];

    /* Init the signal file descriptor */
    event_fd = eventfd(0, 0);
    if (event_fd < 0)
        err(1, "eventfd");

    for (int i = 0; i < N_children; i++) {
        pid[i] = fork();
        if (pid[i] < 0) {
            err(1, "fork [i=%d]", i);
        } else if (pid[i] == 0) {
            srand(i);

            /* mix children */
            usleep(1000 + 1000 * (rand() % 10));
            printf("child %d: started\n", i);

            signal_parent(event_fd);

            /* mix children */
            usleep(100000 + 100000 * (rand() % 10));
            printf("child %d: exiting\n", i);

            exit(0);
        }
    }

    wait_children(event_fd, N_children);
    printf("parent: all children are running, safe to proceed\n");

    for (int i = 0; i < N_children; i++) {
        if (waitpid(pid[i], &status, 0) < 0)
            err(1, "waitpid");
    }
    printf("parent: all children are dead now\n");
}
