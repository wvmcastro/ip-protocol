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

char addLine(ArpNode*, ArpNode*, unsigned char);

char removeLine(ArpNode*, unsigned int);

ArpNode* newLine(unsigned int, unsigned char*, short int, char*);

// This functions returns a pointer to the previous node
// of the node who has the requested ip address
// if Null means that there is no node with the given ip address
ArpNode* searchLine(ArpNode*, unsigned int);

void printLine(ArpNode*, unsigned int);

void printTable(ArpNode*);

#endif
