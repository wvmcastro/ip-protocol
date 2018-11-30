#ifndef __PROTOCOL_HEADERS__
#define __PROTOCOL_HEADERS__

#include <asm/byteorder.h>

struct ether_hdr
{
	unsigned char	ether_dhost[6];	// Destination address
	unsigned char	ether_shost[6];	// Source address
	unsigned short	ether_type;	// Type of the payload
};

// Read RFC 826 to define the ARP struct
struct arp_hdr
{
  unsigned short arp_hd; // hardware type
  unsigned short arp_pr; // protocol type
  unsigned char arp_hdl; // hardware address len
  unsigned char arp_prl; // protocol address len
  unsigned short arp_op; // opcode
  unsigned char arp_sha[6]; // sender hardware address
  unsigned char arp_spa[4]; // sender protocol address
  unsigned char arp_dha[6]; // destination hardware address
  unsigned char arp_dpa[4]; // destination protocol address
};

struct ip_hdr
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	unsigned char	ip_ihl:4,
			ip_v:4;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	unsigned char	ip_ihl:4,
			ip_v:4;
#endif
	unsigned char	ip_tos;		// Type of service
	unsigned short	ip_len;		// Datagram Length
	unsigned short	ip_id;		// Datagram identifier
	unsigned short	ip_offset;	// Fragment offset
	unsigned char	ip_ttl;		// Time To Live
	unsigned char	ip_proto;	// Protocol
	unsigned short	ip_csum;	// Header checksum
	unsigned int	ip_src;		// Source IP address
	unsigned int	ip_dst;		// Destination IP address
};

#endif
