#ifndef __IP_LINKED_LIST__

#define __IP_LINKED_LIST__

#include <semaphore.h>
#include "definitions.h"

// Node of the linked list where the ARP table will be stored
typedef struct LNode
{
  unsigned int dstIP;
  unsigned int gatewayIP; // aka next hop
  unsigned int netmask;
  char ifaceName[MAX_IFNAME_LEN];
  char ifaceID;
  short int ttl;
  sem_t semaphore;
  struct LNode *next;
} IPNode;

char addLine(IPNode*, IPNode*);

char removeLine(IPNode*, unsigned int, unsigned int, unsigned int);

IPNode* newLine(unsigned int, unsigned int, unsigned int, short int, char, char*);

// This functions returns a pointer to the previous node
// of the node who has the requested target and gateway ipAddress and netmask
// if Null means that there is no node with the exact three numbers
IPNode* searchLine(IPNode*, unsigned int, unsigned int, unsigned int);


void printLine(IPNode*, unsigned int);

void printTable(IPNode*);

#endif
