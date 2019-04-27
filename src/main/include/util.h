
#ifndef UTIL_H
#define UTIL_H

#include <utp.h>

typedef struct{
    utp_context * ctx;
    int fd;
} contextpair;

//should be a hashmap later
typedef struct {
    contextpair * contexts;
    size_t size;
    size_t filled;
} contextlist;


void die(char *fmt, ...);
void hexdump(const void *p, size_t len);
void sigint_handler(int number);

void register_contextpair(utp_context * ctx, int fd);
contextpair * get_contextpair_by_context(utp_context * ctx);


#endif
