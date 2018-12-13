#ifndef __COMMON__
#define __COMMON__

#include "../my_interface.h"
unsigned short computeChecksum(unsigned short*, unsigned short);
int sendEthPacket(char*, MyInterface*);

#endif
