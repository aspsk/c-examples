/*
 * Use openat, fstatat to compute size of a given directory.
 * [This implementation works faster than du(1).]
 */

#define _GNU_SOURCE

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <err.h>

static inline int lstatat(DIR *dir, const char *f, struct stat *buf)
{
	return fstatat(dirfd(dir), f, buf, AT_SYMLINK_NOFOLLOW);
}

static inline DIR *opendirat(int fd, const char *dirname)
{
	int dfd;

	if (fd < 0)
		return opendir(dirname);

	dfd = openat(fd, dirname, O_RDONLY|O_DIRECTORY);
	if (dfd < 0)
		return NULL;
	return fdopendir(dfd);
}

static int directory_size_at(int fd, const char *dirname, off64_t *retp)
{
	DIR *dir;
	struct stat buf;
	struct dirent *d;
	off64_t size = 0;
	int retval = -1;

	dir = opendirat(fd, dirname);
	if (!dir)
		return -1;

	if (fstat(dirfd(dir), &buf))
		goto end;
	size += buf.st_size;

	for ( ;; ) {
		errno = 0;
		d = readdir(dir);
		if (!d)
			break;

		if (!strcmp(".", d->d_name) || !strcmp("..", d->d_name))
			continue;

		if (lstatat(dir, d->d_name, &buf))
			goto end;

		if (S_ISDIR(buf.st_mode)) {
			int ret;
			off64_t s;

			ret = directory_size_at(dirfd(dir), d->d_name, &s);
			if (ret < 0)
				goto end;
			size += s;
		} else if (S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)) {
			size += buf.st_size;
		}

	}

	if (errno == 0) {
		*retp = size;
		retval = 0;
	}

end:
	closedir(dir);
	return retval;
}

/* Non-reentrant version */
int directory_size(const char *dirname, off64_t *retp)
{
	return directory_size_at(-1, dirname, retp);
}

#ifndef NDEBUG
/*
 * Example of usage: implementation of the 'du -sb <dir>' command
 */
int main(int argc, char *argv[])
{
	int ret;
	char *dirname;
	off64_t result;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <directory>\n", *argv);
		return 1;
	}
	dirname = argv[1];

	ret = directory_size(dirname, &result);
	if (ret < 0)
		err(1, "directory_size");
	else
		printf("directory '%s' size is %ld\n", dirname, result);

	return 0;
}
#endif
