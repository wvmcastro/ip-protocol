#include "definitions.h"
#include "ip_linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

char addLine(IPNode *table, IPNode *line)
{
  // If the entry is already in the list delet it
  // Just to maintain consistency
  removeLine(table, line->dstIP);

  sem_wait(&(table->semaphore));
  if(table == NULL) return __ERROR__;
  line->next = table->next;
  table->next = line;
  sem_post(&(table->semaphore));
  return __OK__;
}

char removeLine(IPNode *table, unsigned int ipAddress)
{
  // The table is blocked when a deletion is done
  IPNode *prev;
  prev = searchLine(table, ipAddress);

  sem_wait(&(table->semaphore));
  if(prev != NULL)
  {
    IPNode *n = prev->next;

    prev->next = n->next;
    free(n);
    sem_post(&(table->semaphore));
    return __OK__;
  }
  sem_post(&(table->semaphore));
  return __ERROR__;
}

// always returns the previous node to the desired node
IPNode* searchLine(IPNode *table, unsigned int ipAddress)
{
  sem_wait(&(table->semaphore));
  IPNode *n = table;
  while(n->next != NULL)
  {
    if((n->next)->dstIP == ipAddress)
    {
      sem_post(&(table->semaphore));
      return n;
    }
    n = n->next;
  }
  sem_post(&(table->semaphore));
  return NULL;
}

void printLine(IPNode *line, unsigned int lineId)
{
  printf("%10d | ", lineId);

  unsigned int fourBytes = line->dstIP;
  printf("%3u.%3u.%3u.%3u | ", (fourBytes & 0xFF000000)>>24, (fourBytes & 0x00FF0000) >> 16,
                            (fourBytes & 0x0000FF00) >> 8, fourBytes & 0x000000FF);

  fourBytes = line->gatewayIP;
  printf("%3u.%3u.%3u.%3u | ", (fourBytes & 0xFF000000)>>24, (fourBytes & 0x00FF0000) >> 16,
                            (fourBytes & 0x0000FF00) >> 8, fourBytes & 0x000000FF);

  fourBytes = line->netmask;
  printf("%3u.%3u.%3u.%3u | ", (fourBytes & 0xFF000000)>>24, (fourBytes & 0x00FF0000) >> 16,
                            (fourBytes & 0x0000FF00) >> 8, fourBytes & 0x000000FF);

  printf("%22s | ", line->ifaceName);

  printf("%3d\n", line->ttl);
}

void printTable(IPNode *table)
{
  printf("  Entrada  |   Endereço IP   | Endereço Ethernet | TTL\n");
  IPNode *n = table->next;

  unsigned int i = 0;
  while(n != NULL)
  {
    printLine(n, i);
    n = n->next;
    i++;
  }
}

IPNode* newLine(unsigned int dstIP, unsigned int gatewayIP, unsigned int mask, short int ttl, char *ifName)
{
  IPNode *node = (IPNode*) malloc(sizeof(IPNode));
  node->dstIP = dstIP;
  node->gatewayIP = gatewayIP;
  node->netmask = mask;
  node->ttl = ttl;
  if(ifName != NULL) strcpy(node->ifaceName, ifName);
  node->next = NULL;
  sem_init(&(node->semaphore), 0, 1);
  return node;
}
