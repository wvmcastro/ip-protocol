#include "ethernet_protocol.h"
#include "protocol_headers.h"
#include <arpa/inet.h>
#include <string.h>

void fillEthernetHeader(struct ether_hdr *frame, unsigned char *dhost, unsigned char *shost, unsigned short type)
{
  memcpy(frame->ether_dhost, dhost, 6);
  memcpy(frame->ether_shost, shost, 6);
  frame->ether_type = htons(type);
}
