#include "arp_protocol.h"
#include "../definitions.h"
#include "protocol_headers.h"
#include "../communication.h"

#include <stdlib.h>
#include <string.h> // memset
#include <net/if.h> // if_nametoindex
#include <unistd.h> // close function
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

char* buildArpRequest(unsigned int myIP, unsigned char *myMAC, unsigned int dstIP)
{
  unsigned char dstMAC[6] = {0};
  return buildArpPacket(myIP, myMAC, dstIP, dstMAC, ARP_REQUEST);
}

char* buildArpReply(unsigned int myIP, unsigned char *myMAC, unsigned int dstIP, unsigned char *dstMAC)
{
  return buildArpPacket(myIP, myMAC, dstIP, dstMAC, ARP_REPLY);
}

char *buildArpPacket(unsigned int myIP, unsigned char *myMAC,
                     unsigned int dstIP, unsigned char *dstMAC,
                     unsigned short type)
{
  int arpHeaderLen, ethHeaderLen;
  arpHeaderLen = sizeof(struct arp_hdr);
  ethHeaderLen = sizeof(struct ether_hdr);

  char *packet = (char*) malloc(arpHeaderLen + ethHeaderLen);

  struct ether_hdr *eth = (struct ether_hdr*) packet;

  if(type == ARP_REQUEST)
  {
    memset(eth->ether_dhost, 0xFF, 6); // broadcast mac address
  }
  else
  {
    memcpy(eth->ether_dhost, dstMAC, 6); // broadcast mac address
  }

  memcpy(eth->ether_shost, myMAC, 6);
  eth->ether_type = htons(ARP_ETHERTYPE);

  struct arp_hdr *arp_req = (struct arp_hdr*) (packet+ethHeaderLen);
  arp_req->arp_hd = htons(ARP_HW_TYPE);
  arp_req->arp_pr = htons(ARP_PROTOTYPE);
  arp_req->arp_hdl = HW_ADDR_LEN;
  arp_req->arp_prl = PROTOCOL_ADDR_LEN;
  arp_req->arp_op = htons(type);

  // copy source mac ie myMAC and ip
  memcpy(arp_req->arp_sha, myMAC, 6);
  unsigned int aux = htonl(myIP);
  memcpy(arp_req->arp_spa, &aux, 4);

  // copy destination mac and ip
  memcpy(arp_req->arp_dha, dstMAC, 6);
  aux = htonl(dstIP);
  memcpy(arp_req->arp_dpa, &aux, 4);

  return packet;
}

int sendArpPacket(char *packet, MyInterface *iface)
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
