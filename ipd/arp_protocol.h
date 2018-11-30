#ifndef __ARP_PROTOCOL__
#define __ARP_PROTOCOL__

#include "../my_interface.h"


char* buildArpRequest(unsigned int, unsigned char*, unsigned int);
char* buildArpReply(unsigned int, unsigned char*, unsigned int, unsigned char*);
char *buildArpPacket(unsigned int, unsigned char*, unsigned int, unsigned char*, unsigned short);
int sendArpPacket(char*, MyInterface*);

#endif
