#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <utp.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <netdb.h>



void die(char *fmt, ...){
	va_list ap;
	fflush(stdout);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}