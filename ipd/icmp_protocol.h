#ifndef __ICMP_PROTOCOL__
#define __ICMP_PROTOCOL__

#include "protocol_headers.h"

void turnICMPEchoRequestInReply(struct icmp_hdr*, int);

void setICMPTLEMessage(char*, char*, unsigned short);
#endif
