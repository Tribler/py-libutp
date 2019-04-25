#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include <win32netcompat.h>
#else
#include_next <netinet/tcp.h>
#endif