#include "definitions.h"
#include "arp_linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

char addLine(ArpNode *table, ArpNode *line, unsigned char type)
{
  // If the entry is already in the list delet it
  // Just to maintain consistency
  removeLine(table, line->ipAddress);

  sem_wait(&(table->semaphore));
  if(table == NULL) return __ERROR__;
  line->type = type;
  line->next = table->next;
  table->next = line;
  sem_post(&(table->semaphore));
  return __OK__;
}

char removeLine(ArpNode *table, unsigned int ipAddress)
{
  // The table is blocked when a deletion is done
  ArpNode *prev;
  prev = searchLine(table, ipAddress);

  sem_wait(&(table->semaphore));
  if(prev != NULL)
  {
    ArpNode *n = prev->next;

    prev->next = n->next;
    free(n);
    sem_post(&(table->semaphore));
    return __OK__;
  }
  sem_post(&(table->semaphore));
  return __ERROR__;
}

// always returns the previous node to the desired node
ArpNode* searchLine(ArpNode *table, unsigned int ipAddress)
{
  sem_wait(&(table->semaphore));
  ArpNode *n = table;
  while(n->next != NULL)
  {
    if((n->next)->ipAddress == ipAddress)
    {
      sem_post(&(table->semaphore));
      return n;
    }
    n = n->next;
  }
  sem_post(&(table->semaphore));
  return NULL;
}

void printLine(ArpNode *line, unsigned int lineId)
{
  printf("%10d | ", lineId);

  unsigned int ip = line->ipAddress;
  printf("%3u.%3u.%3u.%3u | ", (ip & 0xFF000000)>>24, (ip & 0x00FF0000) >> 16,
                            (ip & 0x0000FF00) >> 8, ip & 0x000000FF);

  unsigned char *mac = line->macAddress;
  printf("%2X:%2X:%2X:%2X:%2X:%2X | ", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  printf("%3d\n", line->ttl);
}

void printTable(ArpNode *table)
{
  printf("  Entrada  |   Endereço IP   | Endereço Ethernet | TTL\n");
  ArpNode *n = table->next;

  unsigned int i = 0;
  while(n != NULL)
  {
    printLine(n, i);
    n = n->next;
    i++;
  }
}

ArpNode* newLine(unsigned int ipAddress, unsigned char *macAddress, short int ttl, char *ifName)
{
  ArpNode *node = (ArpNode*) malloc(sizeof(ArpNode));
  node->ipAddress = ipAddress;

  for(unsigned int i = 0; i < 6; i++)
  {
    node->macAddress[i] = macAddress[i];
  }

  node->ttl = ttl;

  if(ifName != NULL) strcpy(node->ifaceName, ifName);

  node->next = NULL;

  sem_init(&(node->semaphore), 0, 1);
  return node;
}
