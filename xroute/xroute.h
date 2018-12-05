#ifndef __XROUTE__
#define __XROUTE__

#include "../ip_linked_list.h"

char getOperation(const char*);

char showRouteTable();

char addOrRemoveRouteLine(char, const char*, const char*, const char*);

void routeLine2HostByteOrder(IPNode* line);

#endif
