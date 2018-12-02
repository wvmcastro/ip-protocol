#ifndef __XIFCONFIG_SERVER__
#define __XIFCONFIG_SERVER__

#include "../arp_linked_list.h"
#include "../my_interface.h"

void xifconifg_server_run(int,
                          unsigned char, char*,
                          ArpNode*,
                          MyInterface*, int);


void sendIfaces(int, MyInterface*, int);
void sendIface(int, MyInterface*);
void iface2NetworkByteOrder(MyInterface*);
void safeIfaceCopy(MyInterface*, MyInterface*);
void configIface(MyInterface*, const char*, int, unsigned int, unsigned int, ArpNode*);
unsigned char getIfaceIndex(MyInterface*, const char*, int);
void setMTUSize(MyInterface*, const char*, int, unsigned short);
void toggleInterface(char*, unsigned char, MyInterface*, int, ArpNode*);

#endif
