#include "definitions.h"
#include "ip_linked_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

char addLine(IPNode *table, IPNode *line)
{
  if(table == NULL) return __ERROR__;

  // If the entry is already in the list delet it
  // Just to maintain consistency
  removeLine(table, line->dstIP, line->gatewayIP, line->netmask);

  // blocks the table
  sem_wait(&(table->semaphore));

  // adds lines in a sorted way
  IPNode *prev, *current, *aux;
  prev = table;
  current = table->next;
  char found = 0;

  while(current != NULL && !found)
  {
    if(inSameNetwork(current, line) && (line->netmask > current->netmask))
    {
      // if the new line entry has a minor prefix lights up found flag
      found = 1;
    }

    if(!found)
    {
      aux = current->next;
      prev = current;
      current = aux;
    }

  }

  line->next = prev->next;
  prev->next = line;

  // releases the table
  sem_post(&(table->semaphore));

  return __OK__;
}

char inSameNetwork(IPNode *a, IPNode *b)
{
  unsigned int n1, n2;
  if(a->netmask < b->netmask)
  {
    n1 = a->dstIP & a->netmask;
    n2 = b->dstIP & a->netmask;
  }
  else
  {
    n1 = a->dstIP & b->netmask;
    n2 = b->dstIP & b->netmask;
  }
  printf("n1:%u n2:%u\n", n1, n2);
  return n1 == n2;
}

char removeLine(IPNode *table, unsigned int dstIP, unsigned int gatewayIP, unsigned int netmask)
{
  // The table is blocked when a deletion is done
  IPNode *prev;
  prev = searchLine(table, dstIP, gatewayIP, netmask);

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
IPNode* searchLine(IPNode *table, unsigned int dstIP, unsigned int gatewayIP, unsigned int netmask)
{
  sem_wait(&(table->semaphore));
  IPNode *n = table;
  while(n->next != NULL)
  {
    if((n->next)->dstIP == dstIP &&
       (n->next)->gatewayIP == gatewayIP &&
       (n->next)->netmask == netmask)
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

  printf("%s | ", line->ifaceName);

  if(line->ttl == -1)
  {
    printf("Inf\n");
  }
  else
  {
      printf("%3d\n", line->ttl);
  }
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
