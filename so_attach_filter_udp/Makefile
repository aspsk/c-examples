.phony: all clean

CFLAGS ?= -std=c17 -pedantic -Wall -Wextra

all: so_attach_filter_udp

so_attach_filter_udp: so_attach_filter_udp.o

clean:
	${RM} *.o so_attach_filter_udp
