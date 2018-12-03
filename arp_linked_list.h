#ifndef __ARP_LINKED_LIST__

#define __ARP_LINKED_LIST__

#include <semaphore.h>
#include "definitions.h"

// Node of the linked list where the ARP table will be stored
typedef struct lNode
{
  unsigned int ipAddress;
  unsigned char macAddress[6];
  short int ttl;
  unsigned char type;
  char ifaceName[MAX_IFNAME_LEN];
  sem_t semaphore;
  struct lNode *next;
} ArpNode;

char addARPLine(ArpNode*, ArpNode*, unsigned char);

char removeARPLine(ArpNode*, unsigned int);

ArpNode* newARPLine(unsigned int, unsigned char*, short int, char*);

// This functions returns a pointer to the previous node
// of the node who has the requested ip address
// if Null means that there is no node with the given ip address
ArpNode* searchARPLine(ArpNode*, unsigned int);

void printARPLine(ArpNode*, unsigned int);

void printARPTable(ArpNode*);

#endif
