
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
    // const unsigned char *p;
    // ssize_t len, left;

    // left = a->len;
    // p = a->buf;

    // while (left) {
    // 	len = write(STDOUT_FILENO, p, left);
    // 	left -= len;
    // 	p += len;
    // 	debug("Wrote %d bytes, %d left\n", len, left);
    // }
    // utp_read_drained(a->socket);
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
    // utp_socket_stats *stats;

    // switch (a->state) {
    // 	case UTP_STATE_CONNECT:
    // 	case UTP_STATE_WRITABLE:
    // 		write_data();
    // 		break;

    // 	case UTP_STATE_EOF:
    // 		debug("Received EOF from socket\n");
    // 		utp_eof_flag = 1;
    // 		if (utp_shutdown_flag) {
    // 			utp_close(a->socket);
    // 		}
    // 		break;

    // 	case UTP_STATE_DESTROYING:
    // 		debug("UTP socket is being destroyed; exiting\n");

    // 		stats = utp_get_stats(a->socket);
    // 		if (stats) {
    // 			debug("Socket Statistics:\n");
    // 			debug("    Bytes sent:          %d\n",
    // stats->nbytes_xmit); 			debug("    Bytes received:      %d\n",
    // stats->nbytes_recv); 			debug("    Packets received:    %d\n",
    // stats->nrecv); 			debug("    Packets sent:        %d\n", stats->nxmit);
    // 			debug("    Duplicate receives:  %d\n", stats->nduprecv);
    // 			debug("    Retransmits:         %d\n", stats->rexmit);
    // 			debug("    Fast Retransmits:    %d\n",
    // stats->fastrexmit); 			debug("    Best guess at MTU:   %d\n",
    // stats->mtu_guess);
    // 		}
    // 		else {
    // 			debug("No socket statistics available\n");
    // 		}

    // 		s = NULL;
    // 		quit_flag = 1;
    // 		break;
    // }

    return 0;
}

uint64 callback_sendto(utp_callback_arguments *a) {
    // struct sockaddr_in *sin = (struct sockaddr_in *) a->address;

    // debug("sendto: %zd byte packet to %s:%d%s\n", a->len,
    // inet_ntoa(sin->sin_addr), ntohs(sin->sin_port), 			(a->flags &
    // UTP_UDP_DONTFRAG) ? "  (DF bit requested, but not yet implemented)" :
    // "");

    // if (o_debug >= 3)
    // 	hexdump(a->buf, a->len);

    // sendto(fd, a->buf, a->len, 0, a->address, a->address_len);
    return 0;
}

uint64 callback_log(utp_callback_arguments *a) {
    fprintf(stderr, "log: %s\n", a->buf);
    return 0;
}