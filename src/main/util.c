#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <utp.h>
#include <util.h>

contextlist contexts = {NULL,0};

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


void register_contextpair(utp_context * ctx, int fd){
    if(contexts.contexts == NULL){
        contexts.size = 4;
        contexts.contexts = calloc(contexts.size, sizeof(contextpair));
    }

    contexts.contexts[contexts.filled++] = (contextpair){
        ctx,
        fd,
    };

    if(contexts.filled >= contexts.size){
        contexts.size <<= 1;
        contexts.contexts = realloc(contexts.contexts, contexts.size*sizeof(contextpair));
    }
}


contextpair * get_contextpair_by_context(utp_context * ctx){

    for(int i = 0; i < contexts.size; i++){
        if(contexts.contexts[i].ctx == ctx){
            return &contexts.contexts[i];
        }
    }
    return NULL;
}

