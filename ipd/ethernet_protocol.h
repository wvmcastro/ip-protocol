#ifndef __ETHERNET_PROTOCOL__
#define __ETHERNET_PROTOCOL__

#include "protocol_headers.h"

void fillEthernetHeader(struct ether_hdr*, unsigned char*, unsigned char*, unsigned short);

#endif
