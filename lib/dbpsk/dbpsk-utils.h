#ifndef DBPSK_UTILS_H
#define DBPSK_UTILS_H

/* Macros for conversion between host and network byte order
 */
#if defined(__BYTE_ORDER__)
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define htons(n) n
	#define ntohs(n) n
	#define htonl(n) n
	#define ntohl(n) n
#else
	#define htons(n) (((n & 0xFF) << 8) | ((n & 0xFF00) >> 8))
	#define ntohs(n) (((n & 0xFF) << 8) | ((n & 0xFF00) >> 8))
	#define htonl(n) (((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24))
	#define ntohl(n) (((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24))
#endif
#else
	#error "Your compiler doesn't support __BYTE_ORDER__"
#endif

#endif