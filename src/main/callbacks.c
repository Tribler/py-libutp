
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <utp.h>

#include <callbacks.h>
#include <util.h>

uint64 callback_on_read(utp_callback_arguments *a) {
    printf("read something!\n");
    const unsigned char *p;
    ssize_t len, left;

    left = a->len;
    p = a->buf;

    while (left) {
    	len = write(STDOUT_FILENO, p, left);
    	left -= len;
    	p += len;
    	printf("Wrote %ld bytes, %ld left\n", len, left);
    }
    utp_read_drained(a->socket);
    return 0;
}

uint64 callback_on_firewall(utp_callback_arguments *a) {
    printf("firewalling connection or whatever that means?\n");
    return 0;
}

uint64 callback_on_accept(utp_callback_arguments *a) {
    printf("Accepted socket\n");
    return 0;
}

uint64 callback_on_error(utp_callback_arguments *a) {
    // utp_close(s);
    die("Error: %s\n", utp_error_code_names[a->error_code]);
    return 0;
}

uint64 callback_on_state_change(utp_callback_arguments *a) {
    printf("state changed to %d: %s\n", a->state, utp_state_names[a->state]);
    utp_socket_stats *stats;

    switch (a->state) {
    	case UTP_STATE_CONNECT:
    	case UTP_STATE_WRITABLE:
            printf("writiable!\n");
    		// write_data();
    		break;

    	case UTP_STATE_EOF:
            printf("eof");
    		break;

    	case UTP_STATE_DESTROYING:
            printf("shutdown");
    		break;
    }

    return 0;
}

uint64 callback_sendto(utp_callback_arguments *a) {
    printf("broken rn\n");
    struct sockaddr_in *sin = (struct sockaddr_in *) a->address;

    printf(
        "sendto: %zd byte packet to %s:%d%s\n", 
        a->len,
        inet_ntoa(sin->sin_addr), 
        ntohs(sin->sin_port),
        (a->flags & UTP_UDP_DONTFRAG) 
            ? "  (DF bit requested, but not yet implemented)" 
            : ""
    );

   	hexdump(a->buf, a->len);
    int fd = utp_get_state_by_context(a->context)->socket_fd;

    sendto(fd, a->buf, a->len, 0, a->address, a->address_len);
    return 0;
}

uint64 callback_log(utp_callback_arguments *a) {
    fprintf(stderr, "log: %s\n", a->buf);
    return 0;
}