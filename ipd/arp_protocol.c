#include "arp_protocol.h"
#include "../definitions.h"
#include "protocol_headers.h"
#include "../communication.h"
#include "ethernet_protocol.h"

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
  unsigned char dhost[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  if(type != ARP_REQUEST)
  {
    memcpy(dhost, dstMAC, 6); // broadcast mac address
  }

  struct ether_hdr *eth = (struct ether_hdr*) packet;
  fillEthernetHeader(eth, dhost, myMAC, ARP_ETHERTYPE);

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
