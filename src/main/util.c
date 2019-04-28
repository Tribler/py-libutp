#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <utp.h>
#include <util.h>
#include <hashmap.h>


hashmap * contexts = NULL;

void die(char *fmt, ...) {
    va_list ap;
    fflush(stdout);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

void hexdump(const void *p, size_t len) {
    int count = 1;

    while (len--) {
        sleep(0.5);

        if (count == 1)
            fprintf(stderr, "    %p: ", p);

        fprintf(stderr, " %02x", *(unsigned char *)p++ & 0xff);

        if (count++ == 16) {
            fprintf(stderr, "\n");
            count = 1;
        }
    }

    if (count != 1)
        fprintf(stderr, "\n");
}


void sigint_handler(int number){
	printf("caught sigint, exiting.\n");
    exit(0);
}

utp_state * utp_create_default_state(){
    utp_state * res = malloc(1*sizeof(utp_state));
    *res = (utp_state){
        .listenport = "0",
        .listenaddress = "0.0.0.0",
        .connectingport = "4242",
        .connectingaddress = "127.0.0.1",

        .socket_fd = -1, //not yet initialized
        .context=NULL, 

        .listen=1,
        .socket=NULL,
    };

    return res;
}



void utp_store_state(utp_state * state){
    if(contexts == NULL){
        contexts = hashmap_init();
    }

    utp_context * context = state->context;

    hashmap_put(contexts,(void *)context, (void *)state);
};


utp_state * utp_get_state_by_context(utp_context * ctx){
        if(contexts == NULL){
        contexts = hashmap_init();
    }
    return (utp_state *)hashmap_get(contexts,(void *)ctx);
};


void utp_delete_state_by_context(utp_context * ctx){
    if(contexts == NULL){
        contexts = hashmap_init();
    }
    hashmap_delete(contexts,(void *)ctx);
};



