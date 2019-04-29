#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include <win32netcompat.h>
#else
#include_next <netinet/in.h>
#endif

#ifndef LIBCRYPTOCOMPAT_NETINET_IN_H
#define LIBCRYPTOCOMPAT_NETINET_IN_H

#ifdef __ANDROID__
typedef uint16_t in_port_t;
#endif

#endif