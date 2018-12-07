#include <stdio.h>
#include <arpa/inet.h> // ntohl

#include "xroute_server.h"
#include "../communication.h"

void xroute_server_run(int newsockfd,
                       unsigned char opCode, char *message,
                       IPNode* routeTable,
                       MyInterface* ifaces, int numIfaces)
{

  static unsigned int target, gateway, netmask;

  if(opCode == ADD_ROUTE_LINE || opCode == DEL_ROUTE_LINE)
  {
    target = ntohl(*(unsigned int*) message);
    gateway = ntohl(*(unsigned int*)(message+4));
    netmask = ntohl(*(unsigned int*) (message+8));
  }

  switch(opCode)
  {
    case SHOW_ROUTE_TABLE:
      sendRouteTable(newsockfd, routeTable);
      break;
    case ADD_ROUTE_LINE:
      addRoute(routeTable, ifaces, numIfaces, target, gateway, netmask);
      break;
    case DEL_ROUTE_LINE:
      removeLine(routeTable, target, gateway, netmask);
      break;

    default:
      printf("OPERATION NOT SUPPORTED BY IPD (XROUTE SERVER)\n");
  }
}

void sendRouteTable(int socket, IPNode *routeTable)
{
  IPNode *line = routeTable;
  IPNode aux;
  unsigned char IPNodeLen = sizeof(IPNode);
  while(line->next != NULL)
  {
    sem_wait(&(line->next->semaphore));
    aux = *(line->next); // safe copy the line
    sem_post(&(line->next->semaphore));
    routeLine2NetworkByteOrder(&aux);
    _send(socket, (char*)&aux, IPNodeLen);
    line = line->next;
  }
}

void routeLine2NetworkByteOrder(IPNode* line)
{
  line->dstIP = htonl(line->dstIP);
  line->dstIP = htonl(line->dstIP);
  line->gatewayIP = htonl(line->gatewayIP);
  line->netmask = htonl(line->netmask);
  line->ttl = htons(line->ttl);
}

void addRoute(IPNode* routeTable, MyInterface *ifaces, int numIfaces,
              unsigned int target, unsigned int gateway, unsigned netmask)
{
  char index = getIfaceByPrefix(gateway, ifaces, numIfaces);
  char *ifaceName;
  ifaceName = (index != -1) ? ifaces[(unsigned char) index].name : "no_iface";
  addLine(routeTable, newLine(target, gateway, netmask, -1, index, ifaceName));
}

char getIfaceByPrefix(unsigned int gateway, MyInterface* ifaces, int numIfaces)
{
  unsigned int curretNetmask = 0;
  unsigned char i;
  char index;
  index = -1;

  for(i = 0; i < numIfaces; i++)
  {
    if((gateway & ifaces[i].netMask) == (ifaces[i].ipAddress & ifaces[i].netMask))
    {
      if(ifaces[i].netMask >= curretNetmask)
      {
        index = i;
        curretNetmask =  ifaces[i].netMask;
      }
    }
  }

  return index;
}
