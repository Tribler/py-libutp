
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <utp.h>

#include <callbacks.h>
#include <util.h>

#define PORT "4242"
#define LISTEN_ADDR "0.0.0.0"
#define REMOTE_ADDR "127.0.0.1"
#define REMOTE_PORT "4242"

void serve() {
    int error;

    // create a unix socket and check if it was created
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd < 0) {
        die("invalid socket");
    }

    // the hints structure specifies the criteria
    // for selecting a socket address
    // see http://man7.org/linux/man-pages/man3/gai_strerror.3.html
    // res will contain the actual address info
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // get the address to listen on
    if ((error = getaddrinfo(LISTEN_ADDR, PORT, &hints, &res))) {
        die("error while calling getaddrinfo: %s\n", gai_strerror(error));
    }

    if (bind(socket_fd, res->ai_addr, res->ai_addrlen) != 0) {
        die("error while calling bind");
    }

    freeaddrinfo(res);

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(socket_fd, (struct sockaddr *)&sin, &len) != 0) {
        die("couldn't get socket name");
    }
    printf("Bound to local %s:%d\n", inet_ntoa(sin.sin_addr),
           ntohs(sin.sin_port));

    // initialize utp and create an utp socket
    utp_context *ctx = utp_init(2);

    utp_set_callback(ctx, UTP_LOG, &callback_log);
    utp_set_callback(ctx, UTP_SENDTO, &callback_sendto);
    utp_set_callback(ctx, UTP_ON_ERROR, &callback_on_error);
    utp_set_callback(ctx, UTP_ON_STATE_CHANGE, &callback_on_state_change);
    utp_set_callback(ctx, UTP_ON_READ, &callback_on_read);
    utp_set_callback(ctx, UTP_ON_FIREWALL, &callback_on_firewall);
    utp_set_callback(ctx, UTP_ON_ACCEPT, &callback_on_accept);

    // debug options
    utp_context_set_option(ctx, UTP_LOG_NORMAL, 1);
    utp_context_set_option(ctx, UTP_LOG_MTU, 1);
    utp_context_set_option(ctx, UTP_LOG_DEBUG, 1);

    utp_socket *sock = utp_create_socket(ctx);
    // get the address to connect to
    if ((error = getaddrinfo(REMOTE_ADDR, REMOTE_PORT, &hints, &res))) {
        die("error while calling getaddrinfo: %s\n", gai_strerror(error));
    }

    struct sockaddr_in *sin_remote;
    sin_remote = (struct sockaddr_in *)res->ai_addr;
    printf("Connecting to %s:%d\n", inet_ntoa(sin_remote->sin_addr),
           ntohs(sin_remote->sin_port));

    utp_connect(sock, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    // setup complete

    // unsigned char socket_data[4096];

    // poll file descriptors
    struct pollfd p[1];

    p[0].fd = socket_fd;
    p[0].events = POLLIN;

    while (1) {
        int ret = poll(p, 1, 500);
        // error handling
        if (ret < 0) {
            if (errno == EINTR) {
                // printf("Well... something went wrong. Dont worry!\n");
                die("exiting due to sigint probably. remove this in "
                    "production as also "
                    "other things can trigger this.\n");
            } else {
                die("Something went seriously wrong while polling. Bye!");
            }
        } else if (ret == 0) {
            printf("poll timeout\n");
        } else {
            // if()
        }
    }
}

void client() {
    int error;

    // create a unix socket and check if it was created
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd < 0) {
        die("invalid socket");
    }

    // the hints structure specifies the criteria
    // for selecting a socket address
    // see http://man7.org/linux/man-pages/man3/gai_strerror.3.html
    // res will contain the actual address info
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // get the address to listen on
    if ((error = getaddrinfo(LISTEN_ADDR, PORT, &hints, &res))) {
        die("error while calling getaddrinfo: %s\n", gai_strerror(error));
    }

    if (bind(socket_fd, res->ai_addr, res->ai_addrlen) != 0) {
        die("error while calling bind");
    }

    freeaddrinfo(res);

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(socket_fd, (struct sockaddr *)&sin, &len) != 0) {
        die("couldn't get socket name");
    }
    printf("Bound to local %s:%d\n", inet_ntoa(sin.sin_addr),
           ntohs(sin.sin_port));

    // initialize utp and create an utp socket
    utp_context *ctx = utp_init(2);

    utp_set_callback(ctx, UTP_LOG, &callback_log);
    utp_set_callback(ctx, UTP_SENDTO, &callback_sendto);
    utp_set_callback(ctx, UTP_ON_ERROR, &callback_on_error);
    utp_set_callback(ctx, UTP_ON_STATE_CHANGE, &callback_on_state_change);
    utp_set_callback(ctx, UTP_ON_READ, &callback_on_read);
    utp_set_callback(ctx, UTP_ON_FIREWALL, &callback_on_firewall);
    utp_set_callback(ctx, UTP_ON_ACCEPT, &callback_on_accept);

    // debug options
    utp_context_set_option(ctx, UTP_LOG_NORMAL, 1);
    utp_context_set_option(ctx, UTP_LOG_MTU, 1);
    utp_context_set_option(ctx, UTP_LOG_DEBUG, 1);
}

/**
 * Entry point of the program
 */
int main(int argc, char *argv[]) {
    return 0;
}
