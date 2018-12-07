#ifndef __IP_PROTOCOL__
#define __IP_PROTOCOL__

struct ip_hdr createIpHeader(unsigned char,
                             unsigned char,
                             unsigned char,
                             unsigned short,
                             unsigned short,
                             unsigned short,
                             unsigned char,
                             unsigned char,
                             unsigned short,
                             unsigned int,
                             unsigned int);
unsigned char decrementTTL(char*);
unsigned short computeChecksum(unsigned short *);
unsigned char validateChecksum(unsigned short *, unsigned short);
unsigned char isIpV4(unsigned char);
#endif
