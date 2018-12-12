#ifndef __IP_PROTOCOL__
#define __IP_PROTOCOL__

#include "protocol_headers.h"

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
unsigned char validateIPChecksum(struct ip_hdr*);
unsigned char isIpV4(unsigned char);
unsigned char updateTTLandChecksum(struct ip_hdr*);

#endif
