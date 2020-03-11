#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/udp.h>
#include <linux/filter.h>

#include <err.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int sk;
	struct sockaddr_in addr;

	/* In our example we are using a normal UDP socket. When we will attach
	 * a filter, for every packet it will be executed with the packet
	 * context set to the beginning of the UDP header, so, e.g., `ldh [4]`
	 * will load the register A with the length of the datagram. */
	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0)
		err(1, "socket");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(argc > 1 ? atoi(argv[1]) : 1234);
	if (bind(sk, (void *)&addr, sizeof(addr)) < 0)
		err(1, "bind");

	/* This BPF program consists only of one instruction. The definitions
	 * of BPF* macros used in the following statement can be found in
	 * <linux/filter.h>. The only instruction is just `ret 8+2`, where 8
	 * equals to bytes occupied by the UDP header and 2 — is just a random
	 * number of bytes which we chose. */
	struct sock_filter bytecode[] = {
		BPF_STMT(BPF_RET, sizeof(struct udphdr) + 2),
	};
	struct sock_fprog code = { .len = 1, .filter = bytecode };

	/* The setsockopt call is the same as for raw sockets */
	if (setsockopt(sk, SOL_SOCKET, SO_ATTACH_FILTER, &code, sizeof(code)) < 0)
		err(1, "setsockopt");

	for ( ;; ) {
		ssize_t n;
		char buf[1024];

		/* Now recvfrom should always return less than or equal to 2
		 * bytes (we can receive a real UDP datagram which size is less
		 * than 2 bytes). */
		n = recvfrom(sk, buf, sizeof(buf), 0, NULL, NULL);
		if (n < 0)
			err(1, "recvfrom");
		else if (n > 2)
			errx(1, "bug: recvfrom returned %zd bytes", n);
		else
			warnx("received %zd bytes", n);
	}

	return 0;
}
