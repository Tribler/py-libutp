
#ifndef UTIL_H
#define UTIL_H

#include <utp.h>

typedef struct utp_state{
    char * listenport;
    char * listenaddress;
    char * connectingport;
    char * connectingaddress;

    int socket_fd;
    utp_context * context;

    bool listen;
    utp_socket *socket;
}utp_state;


void die(char *fmt, ...);
void hexdump(const void *p, size_t len);
void sigint_handler(int number);
utp_state * utp_create_default_state();
void utp_store_state(utp_state * state);
utp_state * utp_get_state_by_context(utp_context * ctx);
void utp_delete_state_by_context(utp_context * ctx);




#endif
