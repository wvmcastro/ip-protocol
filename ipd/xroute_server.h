#ifndef __XROUTE_SERVER__
#define __XROUTE_SERVER__

#include "../ip_linked_list.h"
#include "../my_interface.h"

void xroute_server_run(int,
                       unsigned char, char*,
                       IPNode* routeTable,
                       MyInterface* ifaces, int numIfaces);

#endif
