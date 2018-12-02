#include <stdio.h>
#include <string.h>
#include <semaphore.h>

#include "xifconfig_server.h"
#include "../my_interface.h"
#include "../definitions.h"
#include "../communication.h"
#include "../arp_linked_list.h"

#define DEBUG 0

void xifconifg_server_run(int newsockfd,
                          unsigned char opCode, char *message,
                          ArpNode* arpTable,
                          MyInterface* ifaces, int numIfaces)
{
  unsigned int ip, mask;
  int n, ifaceNameLen;
  switch(opCode)
  {
    case LIST_IFCES: // lists all ifaces
      sendIfaces(newsockfd, ifaces, numIfaces);
      if(DEBUG == 1)
        printf("IFACES SENT\n");
      break;

    case LIST_IFACE: // lists a single iface
      n = getIfaceIndex(ifaces, message, numIfaces);
      if(n < numIfaces)
      {
        MyInterface aux;
        safeIfaceCopy(&aux, &ifaces[n]);
        if(aux.upDown == IFACE_UP)
          sendIface(newsockfd, &aux);
      }
      break;

    case CONFIG_IFACE:
      // message decode
      ifaceNameLen = strlen(message);
      ip = ntohl(*(unsigned int*)(message+ifaceNameLen+1));
      mask = ntohl(*(unsigned int*)(message+ifaceNameLen+1+4));
      configIface(ifaces, (const char*)message, numIfaces, ip, mask, arpTable);
      break;

    case SET_IFACE_MTU:
      // message decode
      ifaceNameLen = strlen(message);
      unsigned short mtuSize = ntohs(* (unsigned short*)(message+ifaceNameLen+1));
      setMTUSize(ifaces, message, numIfaces, mtuSize);
      break;

    case TURN_IFACE_ON_OFF:
      ifaceNameLen = strlen(message);
      toggleInterface(message, (unsigned char) message[ifaceNameLen+1], ifaces,
                      numIfaces, arpTable);
      break;

    default:
      printf("OPERATION NOT SUPPORTED BY IPD (XIFCONFIG SERVER)\n");
  }
}

void sendIfaces(int socket, MyInterface *ifaces, int numIfaces)
{
	MyInterface aux;
	for(int i = 0; i < numIfaces; i++)
	{
		safeIfaceCopy(&aux, &ifaces[i]);

		if(aux.upDown == IFACE_UP)
			sendIface(socket, &aux);
	}
}

void sendIface(int socket, MyInterface* iface)
{
	//converts ifaces atributes to network byte order
	iface2NetworkByteOrder(iface);
	_send(socket, (char*) iface, sizeof(MyInterface));
}

void iface2NetworkByteOrder(MyInterface *iface)
{
	iface->sockfd = htonl(iface->sockfd);
	iface->ttl = htons(iface->ttl);
  iface->mtu = htons(iface->mtu);
  iface->ipAddress = htonl(iface->ipAddress);
  iface->broadcastAddress = htonl(iface->broadcastAddress);
  iface->netMask = htonl(iface->netMask);
  iface->rxPackets = htonl(iface->rxPackets);
  iface->txPackets = htonl(iface->txPackets);

	int *rx = (int*) &(iface->rxBytes);
	rx[0] = htonl(rx[0]);
	rx[1] = htonl(rx[1]);

	int *tx = (int*) &(iface->txBytes);
	tx[0] = htonl(tx[0]);
	tx[1] = htonl(tx[1]);
}

void safeIfaceCopy(MyInterface *dst, MyInterface *src)
{
	const static unsigned char myInterfaceSize = sizeof(MyInterface);
	sem_wait(&(src->semaphore));
	memcpy(dst, src, myInterfaceSize);
	sem_post(&(src->semaphore));
}


void configIface(MyInterface *ifaces, const char *ifname, int numIfaces,
                 unsigned int ip, unsigned int mask, ArpNode *arpTable)
{
	unsigned char i = getIfaceIndex(ifaces, ifname, numIfaces);
	if(DEBUG == 1)
		printf("CONFIG INTERFACE IP: %u\n", ip);

	if(i < numIfaces) // iface found
	{
		sem_wait(&ifaces[i].semaphore);
		ifaces[i].ipAddress = ip;
		ifaces[i].netMask = mask;

    ArpNode *l = newLine(ip, ifaces[i].macAddress, -1, ifaces[i].name);
    addLine(arpTable, l, STATIC_ENTRY);

		sem_post(&ifaces[i].semaphore);
	}
}

unsigned char getIfaceIndex(MyInterface *ifaces, const char *ifname, int numIfaces)
{
	unsigned char i;
	for(i = 0; i < numIfaces; i++)
	{
		// paglijonson style
		if(strcmp(ifaces[i].name, ifname) == 0) break;
	}
	return i;
}

void setMTUSize(MyInterface *ifaces, const char *ifname, int numIfaces, unsigned short mtu)
{
	unsigned char i = getIfaceIndex(ifaces, ifname, numIfaces);

	if(i < numIfaces)
	{
		if(DEBUG == 1)
			printf("%s MTU size: %u\n", ifname, mtu);

		sem_wait(&ifaces[i].semaphore);
		ifaces[i].mtu = mtu;
		sem_post(&ifaces[i].semaphore);
	}
}

void toggleInterface(char *ifaceName, unsigned char _upDown,
                     MyInterface *ifaces, int numIfaces, ArpNode *arpTable)
{
	unsigned int i = getIfaceIndex(ifaces, ifaceName, numIfaces);
	if(i < numIfaces)
	{
		sem_wait(&ifaces[i].semaphore);
		ifaces[i].upDown = _upDown;
		// deleting the entry from arp arpTable
		removeLine(arpTable, ifaces[i].ipAddress);

		// TODO: delete the interface entry from ip table
		sem_post(&ifaces[i].semaphore);
	}
}
