#ifndef __MY_INTERFACE__
#define __MY_INTERFACE__

#include "definitions.h"
#include <semaphore.h>

typedef struct
{
  unsigned char upDown;
  unsigned int id;
  int		sockfd;
	short		ttl;
  unsigned short mtu;
  char name[MAX_IFNAME_LEN];
  unsigned char macAddress[6];
  unsigned int ipAddress;
  unsigned int broadcastAddress;
  unsigned int netMask;
  unsigned int rxPackets;
  unsigned int txPackets;
  unsigned long int rxBytes;
  unsigned long int txBytes;
  sem_t semaphore;
} MyInterface;

#endif
