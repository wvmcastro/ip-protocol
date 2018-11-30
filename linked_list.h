#ifndef __LINKED_LIST__

#define __LINKED_LIST__

#include <semaphore.h>

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
} Node;

char addLine(Node*, Node*, unsigned char);

char removeLine(Node*, unsigned int);

Node* newLine(unsigned int, unsigned char*, short int, char*);

// This functions returns a pointer to the previous node
// of the node who has the requested ip address
// if Null means that there is no node with the given ip address
Node* searchLine(Node*, unsigned int);

void printLine(Node*, unsigned int);

void printTable(Node*);

#endif
