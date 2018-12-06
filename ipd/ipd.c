#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>
#include <semaphore.h>

#include "../my_interface.h"
#include "../definitions.h"
#include "protocol_headers.h"
#include "../communication.h"
#include "../arp_linked_list.h"
#include "../ip_linked_list.h"
#include "arp_protocol.h"
#include "xifconfig_server.h"
#include "xarp_server.h"
#include "xroute_server.h"

#define MAX_PACKET_SIZE 65536
#define MIN_PACKET_SIZE 20
#define MAX_IFACES	64
#define DEBUG 1

// ########## Global ugly variables ##############
// ##########                       ##############
// interfaces things
MyInterface *my_ifaces;
int numIfaces;
unsigned char *waitingReply;

// arp table
ArpNode arpTable;
short int currentTTL = _DEFAULT_ARP_TTL_;

// routing table
IPNode routeTable;

// mutex for server thread used in xarp res
sem_t xarpServerSemaphore;
// ###############################################
// ###############################################

// Print an Ethernet address
void print_eth_address(char *s, unsigned char *eth_addr)
{
	printf("%s %02X:%02X:%02X:%02X:%02X:%02X\n", s,
	       eth_addr[0], eth_addr[1], eth_addr[2],
	       eth_addr[3], eth_addr[4], eth_addr[5]);
}

// Bind a socket to an interface
int bind_iface_name(int fd, char *iface_name)
{
	return setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, iface_name, strlen(iface_name));
}

void get_iface_info(int sockfd, char *ifname, MyInterface *ifn)
{
	struct ifreq s;

	strcpy(s.ifr_name, ifname);
	if (0 == ioctl(sockfd, SIOCGIFHWADDR, &s))
	{
		memcpy(ifn->macAddress, s.ifr_addr.sa_data, HW_ADDR_LEN);
		ifn->sockfd = sockfd;
		strcpy(ifn->name, ifname);
	}
	else
	{
		perror("Error getting MAC address");
		exit(1);
	}
}

// Print the expected command line for the program
void print_usage()
{
	printf("\nxarpd <interface> [<interfaces>]\n");
	exit(1);
}

void arpPacketHandler(char *packet, int len, MyInterface *iface)
{
	struct arp_hdr *arp = (struct arp_hdr*) packet;
	unsigned short type = ntohs(arp->arp_op);
	unsigned int arp_dpa = ntohl(*(unsigned int*) arp->arp_dpa);
	unsigned int arp_spa = ntohl(*(unsigned int*) arp->arp_spa);

	if (type == ARP_REQUEST)
	{
		iface->rxPackets++;
		iface->rxBytes += len;

		// copy and paste code sorry. I'm in a hurry, but you don't have to worry
		// searchs if some ie has the ip ip address requested
		unsigned char i;
		unsigned int ifaceIP;
		unsigned char ifaceMAC[6];
		for(i = 0; i < numIfaces; i++)
		{
			sem_wait(&my_ifaces[i].semaphore);
			ifaceIP = my_ifaces[i].ipAddress;
			// ifaceNetmask = my_ifaces[i].netMask;
			memcpy(ifaceMAC, my_ifaces[i].macAddress, 6);
			sem_post(&my_ifaces[i].semaphore);

			if(ifaceIP == arp_dpa) break;
		}

		if(i < numIfaces) // there is an interface with the destination protocol address
		{
			if(DEBUG == 1)
				printf("%s: THIS REQUEST WAS FOR ME!\n", my_ifaces[i].name);

			// So we answer !
			char *reply = buildArpReply(my_ifaces[i].ipAddress, my_ifaces[i].macAddress, arp_spa, arp->arp_sha);
			sendArpPacket(reply, iface);
			free(reply);
		}
	}
	else
	{
		if(type == ARP_REPLY)
		{
				if(DEBUG == 1)
				{
					printf("%s: ARP REPLY RECEIVED ", iface->name);
					print_eth_address("FROM", arp->arp_sha);
				}

				if(arp_dpa == iface->ipAddress)
				{
					if(DEBUG == 1)
						printf("%s: THIS REPLY WAS FOR ME!\n", iface->name);

					// adds to list and wakes server thread
					ArpNode *_newARPLine = newARPLine(arp_spa, arp->arp_sha, currentTTL, iface->name);
					addARPLine(&arpTable, _newARPLine, DYNAMIC_ENTRY);

					// Just some extra careful
					if(waitingReply[iface->id] != 0)
					{
						sem_post(&xarpServerSemaphore);
					}
				}
		}
	}
}

ipPacketHandler(unsigned char *packet, int len, MyInterface *iface)
{
	// do nothing for know
}

// Break this function to implement the ARP functionalities.
void doProcess(unsigned char* packet, int len, MyInterface *iface)
{
	if(!len || len < MIN_PACKET_SIZE)
		return;

	struct ether_hdr* eth = (struct ether_hdr*) packet;
	if(htons(0x0806) == eth->ether_type) // is a arp packet
	{
		arpPacketHandler(packet+14, len-14, iface);
	}
	else if(htons(0x0800) == eth->ether_type)
	{
		ipPacketHandler(packet+14, len-14, iface);
	}
	// Ignore if it is not an ARP packet
}


// This function should be one thread for each interface.
void read_iface(void *arg)
{
  MyInterface *ifn;
  ifn = (MyInterface*) arg;
	socklen_t	saddr_len;
	struct sockaddr	saddr;
	unsigned char	*packet_buffer;
	int n;

	saddr_len = sizeof(saddr);
	packet_buffer = malloc(MAX_PACKET_SIZE);

	if (!packet_buffer)
	{
		printf("\nCould not allocate a packet buffer\n");
		exit(1);
	}

	while(1)
	{
		n = recvfrom(ifn->sockfd, packet_buffer, MAX_PACKET_SIZE, 0, &saddr, &saddr_len);
		if(n < 0)
		{
			fprintf(stderr, "ERROR: %s\n", strerror(errno));
			exit(1);
		}

		// isso pode dar errado, talvez tenha que por um mutex
		if(ifn->upDown == IFACE_UP)
			doProcess(packet_buffer, n, ifn);
	}
}

void setupIfces(int numIfaces, char **argv)
{
	int sockfd;

	for (int i = 1; i < numIfaces+1; i++)
	{
		sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
		if(sockfd < 0)
		{
			fprintf(stderr, "ERROR: %s\n", strerror(errno));
			exit(1);
		}

		if (bind_iface_name(sockfd, argv[i]) < 0)
		{
			perror("Server-setsockopt() error for SO_BINDTODEVICE");
			printf("%s\n", strerror(errno));
			close(sockfd);
			exit(1);
		}
		get_iface_info(sockfd, argv[i], &my_ifaces[i-1]);
		my_ifaces[i-1].id = i-1;
		my_ifaces[i-1].mtu = 1500;
		my_ifaces[i-1].upDown = IFACE_UP;
		sem_init(&my_ifaces[i-1].semaphore, 0, 1);
	}
}

void server()
{
	unsigned char BUFFERSIZE = 255;
	char buffer[BUFFERSIZE];

	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  int sockfd = _socket(AF_INET, SOCK_STREAM, 0);
	_bind(&sockfd, (struct sockaddr*) &serv_addr);
	_listen(sockfd, LISTEN_ENQ);

	int n, k, newsockfd;
	char opCode;
	char *message; // aux to message decoding
	unsigned char messageLen;

	if(DEBUG == 1)
		printf("SERVER THREAD IS RUNNING\n");

	while(1)
	{
		if(DEBUG == 1)
			printf("READY TO ACCEPT\n");

		n = 0;
		memset(&cli_addr, 0, sizeof(struct sockaddr_in));
		newsockfd = _accept(sockfd, (struct sockaddr*) &cli_addr);
		do
		{
			k = _recv(newsockfd, buffer+n, BUFFERSIZE-n);
			if(n == 0) messageLen = buffer[0];
			n += k;
		} while(k < messageLen);

		if(n > 0)
		{
			opCode = buffer[1];
			message = buffer + 2;

			if(DEBUG == 1)
				printf("OPCODE: %d\n", opCode);

			if(opCode < 50)
			{
				xifconifg_server_run(newsockfd, opCode, message, &arpTable, my_ifaces, numIfaces);
			}
			else if(opCode >= 50 && opCode < 100)
			{
				xarp_server_run(newsockfd, &xarpServerSemaphore,
												opCode, message,
												&arpTable, &currentTTL,
												my_ifaces, numIfaces, waitingReply);
			}
			else if(opCode >= 100 && opCode < 150)
			{
				xroute_server_run(newsockfd, opCode, message, &routeTable, my_ifaces, numIfaces);
			}
			else
			{
				printf("ERROR: OPCODE NOT RECOGNIZED\n");
			}
		}
		close(newsockfd);
	}
}

void decrementArpTTL()
{
	ArpNode *line;
	ArpNode *next;
	line = arpTable.next;
	while (line != NULL)
	{
		next = line->next;

		if(line->ttl != -1)
			line->ttl--;

		if (line->ttl== 0)
		{
			removeARPLine(&arpTable, line->ipAddress);
		}
		line = next;
	}
}

void decrementer()
{
  while(1)
	{
    sleep(1);
		decrementArpTTL();
  }
}

void initTables()
{
	arpTable.next = NULL;
	routeTable.next = NULL;
	sem_init(&(arpTable.semaphore), 0, 1);
	sem_init(&(routeTable.semaphore), 0, 1);
}

// main function
int main(int argc, char** argv)
{

	initTables();
	sem_init(&xarpServerSemaphore, 0 , 0);
	if (argc < 2)
		print_usage();

	numIfaces = (argc-1 < MAX_IFACES+1) ? argc-1 : MAX_IFACES;

  pthread_t tid[numIfaces];
  pthread_t ttlDecrementer;

	my_ifaces = (MyInterface*) malloc(numIfaces * sizeof(MyInterface));
	memset(my_ifaces, 0, numIfaces * sizeof(MyInterface));

	waitingReply = (unsigned char *) malloc(numIfaces);
	memset(waitingReply, 0, numIfaces);

	setupIfces(numIfaces, argv);

	// This thread will be responsible for answer xarp and xifconfig demands
	pthread_create(&tid[argc - 1], NULL, (void*) server, NULL);
	pthread_create(&ttlDecrementer, NULL, (void*) decrementer, NULL);

	int i;
	for (i = 0; i < numIfaces; i++)
	{
		if(DEBUG == 1)
			print_eth_address(my_ifaces[i].name, my_ifaces[i].macAddress);

		// Create one thread for each interface. Each thread should run the function read_iface.
    pthread_create(&(tid[i]), NULL, (void*) read_iface, (void *) &my_ifaces[i]);
	}

  for (i = 0; i < argc - 1; i++)
  {
    pthread_join(tid[i], NULL);
  }

	pthread_join(tid[argc-1], NULL);
  pthread_join(ttlDecrementer, NULL);

	return 0;
}
