#ifndef __XARP_SERVER__
#define __XARP_SERVER__

#include <semaphore.h>


#include "../arp_linked_list.h"
#include "../my_interface.h"

void xarp_server_run(int,
                     sem_t*,
                     unsigned char, char*,
                     ArpNode*, short int*,
                     MyInterface*, int, unsigned char*);

void sendLines(int, ArpNode*);
void line2NetworkByteOrder(ArpNode*);
void resolveIP(unsigned int, ArpNode*, MyInterface*, int, unsigned char*, sem_t*, int);


#endif
