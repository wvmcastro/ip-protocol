#include <unistd.h>
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <net/if.h> // if_nametoindex
#include <unistd.h> // close function
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include "protocol_headers.h"
#include "../communication.h"
unsigned short computeChecksum(unsigned short *buffer, unsigned short size)
{
  // the size must be in 16bit words
  unsigned long sum = 0;

  for(int i = 0; i < size; i++)
  {
    sum += *buffer++;
    if (sum & 0xFFFF0000)
    {
      sum &= 0xFFFF;
      sum++;
    }
  }
  return ~(sum & 0xFFFF);
}

int sendEthPacket(char *packet, MyInterface *iface)
{
  struct sockaddr_ll device;
  memset(&device, 0, sizeof(struct sockaddr_ll)); // just for safety

  // get iface index through its name
  if((device.sll_ifindex = if_nametoindex(iface->name)) == 0)
  {
    exit(1);
  }

  // Prepares device structure
  device.sll_family = AF_PACKET;
  memcpy(device.sll_addr, iface->macAddress, HW_ADDR_LEN);
  device.sll_halen = HW_ADDR_LEN;

  // Instantiating a socket to send the packet
  int socket = _socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

  // Sends the packet
  unsigned int packetLen = sizeof(struct arp_hdr) + sizeof(struct ether_hdr);
  int bytes = sendto(socket, packet, packetLen, 0, (struct sockaddr*) &device, sizeof(struct sockaddr_ll));
  close(socket);

  if(bytes <= 0) exit(1);

  // counting sent packet and sent bytes
  iface->txPackets++;
  iface->txBytes += bytes;

  if(bytes != packetLen) return __ERROR__;

  return __OK__;
}

/*
colocar 3 hosts abc
configurar o b como roteador
de A tentar pingar em C

*/
