#include "ip_protocol.h"
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

// this function decrements the ttl of an ip header

struct ip_hdr createIpHeader(unsigned char ip_hdl,
                             unsigned char ip_v,
                             unsigned char ip_tos,
                             unsigned short ip_len,
                             unsigned short ip_id,
                             unsigned short ip_offset,
                             unsigned char ip_ttl,
                             unsigned char ip_proto,
                             unsigned short ip_csum,
                             unsigned int ip_src,
                             unsigned int ip_dst)
{
  struct ip_hdr ipHeader;
  ipHeader.ip_hdl = ip_hdl;
  ipHeader.ip_v = ip_v;
  ipHeader.ip_tos = ip_tos;
  ipHeader.ip_len = ip_len;
  ipHeader.ip_id = ip_id;
  ipHeader.ip_offset = ip_offset;
  ipHeader.ip_proto = ip_proto;
  ipHeader.ip_csum = ip_csum;
  ipHeader.ip_src = ip_src;
  ipHeader.ip_dst = ip_dst;

  return ipHeader;
}
unsigned char decrementTTL(char* buffer)
{
  struct ip_hdr* ipHeader = (struct ip_hdr*)buffer;
  ipHeader->ip_ttl -= 1;
  if(ipHeader->ip_ttl == 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

unsigned short computeChecksum(unsigned short * buffer)
{
  unsigned short * aux = buffer;
  struct ip_hdr ipHeader = (struct ip_hdr)aux;
  unsigned long sum = 0;
  int cont = 0;
  cont = ipHeader.ihl * 2;
  while(cont--)
  {
    sum += *aux++;
    if (sum & 0xFFFF0000)
    {
      sum &= 0xFFFF;
      sum++;
    }
  }
  return ~(sum & 0xFFFF);
}

unsigned char validateChecksum(unsigned short * buff, unsigned short receivedChecksum)
{
  unsigned short computedChecksum = computeChecksum(buff);
  return (computeChecksum == receivedChecksum);
}

unsigned char isIpV4(unsigned char ip_v)
{
  return ip_v == 4;
}

void handleIpPacket(char* buffer)
{
  struct ip_hdr * ipHeader = (struct ip_hdr*)buffer;
  unsigned short checksum = ntohs(ipHeader->checksum);
  ipHeader->checksum = 0;
  if validateChecksum((unsigned short *)buffer, checksum)
  {
    if (isIpV4(ipHeader->ip_v))
    {
      if (decrementTTL(buffer))
      {
        checksum = computeChecksum((unsigned short *)buffer);
        ipHeader->checksum = htons(checksum);

        //verify the table to see the
      }
      else
      {
        //generate icmp time exceeded
      }
    }
  }
}
