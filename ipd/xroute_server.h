#ifndef __XROUTE_SERVER__
#define __XROUTE_SERVER__

#include "../ip_linked_list.h"
#include "../my_interface.h"

void xroute_server_run(int,
                       unsigned char, char*,
                       IPNode*,
                       MyInterface*, int);

void sendRouteTable(int, IPNode*);
void routeLine2NetworkByteOrder(IPNode*);
void addRoute(IPNode*, MyInterface*, int, unsigned int, unsigned int, unsigned);
char getIfaceByPrefix(unsigned int, unsigned int, MyInterface*, int);
#endif
