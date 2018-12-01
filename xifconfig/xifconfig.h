#ifndef __XIFCONFIG__
#define __XIFCONFIG__

#include <netinet/in.h>
#include "../my_interface.h"

void makeNewSocketAndConnect(int*, struct sockaddr_in*);

void printInterface(MyInterface*);

void listIfaces(char*);

void configIface(const char*, const char*, const char*);

void setMTUSize(const char*, unsigned short);

#endif
