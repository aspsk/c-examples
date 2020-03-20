.phony: all clean

progs := seccomp_lib

CFLAGS ?= -std=c17 -Wall -Wextra

LDFLAGS_seccomp_lib = -lseccomp

all: ${progs}

%: %.o
	${CC} -o $@ $^ ${LDFLAGS} ${LDFLAGS_$@}

seccomp_lib: seccomp_lib.o

clean:
	${RM} *.o ${progs}
