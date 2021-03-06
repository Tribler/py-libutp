
#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#include <utp.h>

#include <callbacks.h>
#include <hashmap.h>
#include <util.h>

int error;

void init(utp_state * config){

    //handle interrupts
    struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = sigint_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);


    // create a unix socket and check if it was created

    if(config->socket_fd == -1){
        config->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    }
    if (config->socket_fd < 0) {
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
    if ((error = getaddrinfo(config->listenaddress, config->listenport, &hints, &res))) {
        die("error while calling getaddrinfo: %s\n", gai_strerror(error));
    }

    
    if (bind(config->socket_fd, res->ai_addr, res->ai_addrlen) != 0) {
        die(">error while calling bind (%s)\n", strerror(errno));
    }

    freeaddrinfo(res);

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(config->socket_fd, (struct sockaddr *)&sin, &len) != 0) {
        die("couldn't get socket name");
    }
    printf("Bound to local %s:%d\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));


    // initialize utp and create an utp socket
    config->context = utp_init(2);
    //register the config so it can be looked up by its context
    utp_store_state(config);

    // set up callbacks mostly for debug but python will want these too
    utp_set_callback(config->context, UTP_LOG, &callback_log);
    utp_set_callback(config->context, UTP_SENDTO, &callback_sendto);
    utp_set_callback(config->context, UTP_ON_ERROR, &callback_on_error);
    utp_set_callback(config->context, UTP_ON_STATE_CHANGE, &callback_on_state_change);
    utp_set_callback(config->context, UTP_ON_READ, &callback_on_read);
    utp_set_callback(config->context, UTP_ON_FIREWALL, &callback_on_firewall);
    utp_set_callback(config->context, UTP_ON_ACCEPT, &callback_on_accept);

    // debug options
    utp_context_set_option(config->context, UTP_LOG_NORMAL, 1);
    utp_context_set_option(config->context, UTP_LOG_MTU, 1);
    utp_context_set_option(config->context, UTP_LOG_DEBUG, 1);


    if(!config->listen){

        // set up the sending socket. everything above was just for listening
        config->socket = utp_create_socket(config->context);

        // get the address to connect to
        if ((error = getaddrinfo(config->connectingaddress, config->connectingport, &hints, &res))) {
            die("error while calling getaddrinfo: %s\n", gai_strerror(error));
        }

        struct sockaddr_in *sin_remote;
        sin_remote = (struct sockaddr_in *)res->ai_addr;
        printf("Connecting to %s:%d\n", inet_ntoa(sin_remote->sin_addr),
            ntohs(sin_remote->sin_port));

        utp_connect(config->socket, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
    }
}


void serve(char *port) {

    utp_state * config = utp_create_default_state();
    config->listenport = port;
    

    init(config);


    


    // // setup complete

    unsigned char socket_data[4096];
    // receiving address
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(src_addr);

    // which file descriptor to poll and what to wait for (input/POLLIN on the
    // socket)
    struct pollfd p[1];

    p[0].fd = config->socket_fd;
    p[0].events = POLLIN;

    while (1) {
        int ret = poll(p, 1, 500);
        // error handling
        if (ret < 0) {
            if (errno == EINTR) {
                printf("Well... something went wrong. Dont worry!\n");
            } else {
                die("Something went seriously wrong while polling. Bye!");
            }
        } else if (ret == 0) {
            // socket timed out
            printf("poll timeout, retrying...\n");
        } else {
            while (1) {

                // receive a byte
                if ((p[0].revents & POLLIN) == POLLIN) {
                    ssize_t len = recvfrom(
                        config->socket_fd, socket_data, sizeof(socket_data),
                        MSG_DONTWAIT, (struct sockaddr *)&src_addr, &addrlen);
                    if (len < 0) {
                        if (errno == EAGAIN ||
                            errno == EWOULDBLOCK) { // connection ended
                            utp_issue_deferred_acks(config->context);
                            break;
                        } else {
                            // serious error
                            die("receiving failed");
                        }
                    }

                    // everything was fine, we received something
                    // which is in the socket_data string
                    printf("Received %zd byte UDP packet from %s:%d\n", len,
                           inet_ntoa(src_addr.sin_addr),
                           ntohs(src_addr.sin_port));
                    hexdump(socket_data, len);
                    
                    if (! utp_process_udp(config->context, socket_data, len, (struct sockaddr *)&src_addr, addrlen)){
                        printf("UDP packet not handled by UTP.  Ignoring.\n");
                    }
                }
            }
        }
        utp_check_timeouts(config->context);
    }
}

void client(char *addr, char *port, char *whattosend) {
    utp_state * config = utp_create_default_state();
    config->listen = 0;
    config->connectingaddress = "127.0.0.1";
    config->connectingport = "8000";

    init(config);    


    // setup complete

    unsigned char socket_data[4096];
    // receiving address
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(src_addr);

    // which file descriptor to poll and what to wait for (input/POLLIN on the
    // socket)
    struct pollfd p[1];

    p[0].fd = config->socket_fd;
    p[0].events = POLLIN;


    size_t datalength = strlen(whattosend);
    char *notyetsentdata = whattosend; // all

    while (notyetsentdata < whattosend + datalength) {
        int ret = poll(p, 1, 500);
        // error handling
        if (ret < 0) {
            if (errno == EINTR) {
                printf("Well... something went wrong. Dont worry!\n");
            } else {
                die("Something went seriously wrong while polling. Bye!");
            }
        } else if (ret == 0) {
            // socket timed out
            printf("poll timeout, retrying...\n");
        } else {
            while (1) {

                // receive a byte
                if ((p[0].revents & POLLIN) == POLLIN) {
                    ssize_t len = recvfrom(
                        config->socket_fd, socket_data, sizeof(socket_data),
                        MSG_DONTWAIT, (struct sockaddr *)&src_addr, &addrlen);
                    if (len < 0) {
                        if (errno == EAGAIN ||
                            errno == EWOULDBLOCK) { // connection ended
                            utp_issue_deferred_acks(config->context);
                            break;
                        } else {
                            // serious error
                            die("receiving failed");
                        }
                    }

                    // everything was fine, we received something
                    // which is in the socket_data string
                    printf("Received %zd byte UDP packet from %s:%d\n", len,
                           inet_ntoa(src_addr.sin_addr),
                           ntohs(src_addr.sin_port));
                    hexdump(socket_data, len);
                    
                    if (! utp_process_udp(config->context, socket_data, len, (struct sockaddr *)&src_addr, addrlen)){
                        printf("UDP packet not handled by UTP.  Ignoring.\n");
                    }
                }
            }
        
        
            // number of bytes sent this time.
            size_t sent;
            // write to socked_fd with whattosend. last argument is the  left in
            // notyetsentdata. basically it's the pointer to the end of the
            // whattosend string minus the pointer to the start of notyetsent.
            // the loop stops when these pointers are equal.
            sent = utp_write(config->socket, notyetsentdata,
                            whattosend + datalength - notyetsentdata);
            if (sent == 0) {
                printf("socket no longer writable\n");
                sleep(1);
                continue;
            }

            // keep track of how many bytes of the not yet sent data has been sent
            // this call so we can pick up again later
            notyetsentdata += sent;

            if (notyetsentdata == whattosend + datalength) {
                // say we are done. next iteration we exit.
                printf("wrote %zd bytes; buffer now empty\n", sent);
            } else {
                printf("wrote %zd bytes; %ld bytes left in buffer\n", sent,
                    whattosend + datalength - notyetsentdata);
            }
        }
    }

    utp_close(config->socket);
    return;
}

/**
 * Entry point of the program
 */
int main(int argc, char *argv[]) {
    client("127.0.0.1","8000","hello");
    // serve("8000");


}
