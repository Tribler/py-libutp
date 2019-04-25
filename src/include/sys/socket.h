#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include <win32netcompat.h>
#else
#include_next <sys/socket.h>
#endif

#if !defined(SOCK_NONBLOCK) || !defined(SOCK_CLOEXEC)
#define SOCK_CLOEXEC 0x8000  /* set FD_CLOEXEC */
#define SOCK_NONBLOCK 0x4000 /* set O_NONBLOCK */
int bsd_socketpair(int domain, int type, int protocol, int socket_vector[2]);
#define socketpair(d, t, p, sv) bsd_socketpair(d, t, p, sv)
#endif