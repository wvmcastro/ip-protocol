#include <stdio.h> // printf()
#include <stdlib.h> // free()
#include <errno.h> // ETIMEDOUT
#include <arpa/inet.h>
#include <time.h> // used to build the timeout argument for sem_timedwait
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>

#include "xarp_server.h"

#include "../arp_linked_list.h"
#include "../communication.h"
#include "../my_interface.h"
#include "arp_protocol.h"
#include "protocol_headers.h"
#include "common.h"

#define DEBUG 1

void xarp_server_run(int newsockfd,
                     sem_t *xarpServerSemaphore,
                     unsigned char opCode, char *message,
                     ArpNode* arpTable, short int *currentTTL,
                     MyInterface* ifaces, int numIfaces, unsigned char *waitingReply)
{
  unsigned int ip;
  short int _ttl;
  switch(opCode)
  {
    case ADD_ARP_LINE:
      //adds a new line on arp table
      ip = ntohl(*(unsigned int*)message);
      message += 4 + 6;
      _ttl = ntohs(*(short int*)message);
      ArpNode *l = newARPLine(ip, (unsigned char*)(message - 6), _ttl, NULL);
      addARPLine(arpTable, l, STATIC_ENTRY);
      break;

    case SHOW_ARP_TABLE:
      sendLines(newsockfd, arpTable);
      break;

    case DEL_ARP_LINE:
      ip = ntohl(*(unsigned int*) message);
      removeARPLine(arpTable, ip);
      break;

    case RES_IP:
      ip = ntohl(*(unsigned int*) message);
      resolveIP(ip, arpTable, ifaces, numIfaces, waitingReply, xarpServerSemaphore, newsockfd);
      break;

    case SET_ARP_TTL:
      _ttl = ntohs(*(short int*) message);
      *currentTTL = _ttl;
      break;

    default:
      printf("OPERATION NOT SUPPORTED BY XARPD (XARP SERVER)\n");
  }
}

void sendLines(int socket, ArpNode* arpTable)
{
  ArpNode *line = arpTable;
	ArpNode aux;
  unsigned int lineLen = sizeof(ArpNode);
  while (line->next != NULL)
  {
		aux = *(line->next);
    line2NetworkByteOrder(&aux);
    _send(socket, (char*) &aux, lineLen);
    line = line->next;
  }
}

void line2NetworkByteOrder(ArpNode *line)
{
  line->ipAddress = htonl(line->ipAddress);
  line->ttl = htons(line->ttl);
}

void resolveIP(unsigned int ip, ArpNode *arpTable,
               MyInterface *ifaces, int numIfaces, unsigned char *waitingReply,
               sem_t *xarpServerSemaphore,
               int socket)
{
	// first thing is to look for the ip at the arp table
	ArpNode *line = searchARPLine(arpTable, ip);

	if(line == NULL) // line not found
	{
		// searchs the ie in the same network of the requested ip address
		unsigned char i;
    unsigned char upOrDown;
		unsigned int ifaceIP, ifaceNetmask;
		unsigned char ifaceMAC[6];
		for(i = 0; i < numIfaces; i++)
		{
			sem_wait(&ifaces[i].semaphore);
      upOrDown = ifaces[i].upDown;
			ifaceIP = ifaces[i].ipAddress;
			ifaceNetmask = ifaces[i].netMask;
			memcpy(ifaceMAC, ifaces[i].macAddress, 6);
			sem_post(&ifaces[i].semaphore);

      if(upOrDown == IFACE_UP && ifaceNetmask != 0 )
			   if((ifaceIP & ifaceNetmask) == (ip & ifaceNetmask)) break;
		}

		if(i < numIfaces) // ie some iface's network matches with ip resquested network
		{
			waitingReply[i] = 1;

			char *request = buildArpRequest(ifaceIP, ifaceMAC, ip);
			sendEthPacket(request, &ifaces[i],  sizeof(struct arp_hdr) + sizeof(struct ether_hdr));
			free(request);

			if(DEBUG == 1)
				printf("ARP REQUEST SENT BY %s\n", ifaces[i].name);

			// get time info
			clockid_t clockID = CLOCK_REALTIME; // this clock has the time in seconds
																					 // and nanoseconds since THE EPOCH
																					 // what happens when the representation ends ?? POW

			struct timespec ts; //  struct to stores the time
			clock_gettime(clockID, &ts);
			ts.tv_sec += ARP_TIMEOUT;

			// waits for an answer
			// locks the interface for receiving packets
			int ret = sem_timedwait(xarpServerSemaphore, &ts);
			waitingReply[i] = 0;

			if(ret != ETIMEDOUT) // ie: timeout was not exceeded
			{
				line = searchARPLine(arpTable, ip);
			}
		}
	}

	char response[9];
	if(line == NULL) // ie error
	{
		response[0] = __ERROR__;
	}
	else
	{
		response[0] = __OK__;
		short int ttl = htons(line->next->ttl);
		memcpy(response+1, line->next->macAddress, 6);
		memcpy(response+7, &ttl, 2);
	}
	_send(socket, (char*) &response, 9);
}
