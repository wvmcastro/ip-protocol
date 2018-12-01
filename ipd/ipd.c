#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h> // used to build the timeout argument for sem_timedwait

#include "../my_interface.h"
#include "../definitions.h"
#include "protocol_headers.h"
#include "../communication.h"
#include "../linked_list.h"
#include "arp_protocol.h"

#define MAX_PACKET_SIZE 65536
#define MIN_PACKET_SIZE 20
#define MAX_IFACES	64
#define DEBUG	1

// ########## Global ugly variables ##############
// ##########                       ##############
// interfaces things
MyInterface *my_ifaces;
sem_t *ifaceMutexes;
int numIfaces;
unsigned char *waitingReply;

// arp table
Node arpTable;
short int currentTTL = _DEFAULT_ARP_TTL_;

// mutex for server thread used in xarp res
sem_t serverSemaphore;
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


// Break this function to implement the ARP functionalities.
void doProcess(unsigned char* packet, int len, MyInterface *iface)
{
	if(!len || len < MIN_PACKET_SIZE)
		return;

	struct ether_hdr* eth = (struct ether_hdr*) packet;
	if(htons(0x0806) == eth->ether_type) // is a arp packet
	{
    struct arp_hdr *arp = (struct arp_hdr*) (packet + 14);
		unsigned short type = ntohs(arp->arp_op);
		unsigned int arp_dpa = * (unsigned int *) arp->arp_dpa;
		unsigned int arp_spa = * (unsigned int *) arp->arp_spa;

    if (type == ARP_REQUEST)
    {
			iface->rxPackets++;
			iface->rxBytes += len;

			// copy and paste code sorry. I'm in a hurry, but you don't have to worry
			// searchs if some ie has the ip ip address requested
			unsigned char i;
			unsigned int ifaceIP, ifaceNetmask;
			unsigned char ifaceMAC[6];
			for(i = 0; i < numIfaces; i++)
			{
				sem_wait(&ifaceMutexes[i]);
				ifaceIP = my_ifaces[i].ipAddress;
				ifaceNetmask = my_ifaces[i].netMask;
				memcpy(ifaceMAC, my_ifaces[i].macAddress, 6);
				sem_post(&ifaceMutexes[i]);

				if(ifaceIP == ntohl(arp_dpa)) break;
			}

			if(i < numIfaces) // there is an interface with the destination protocol address
			{
				if(DEBUG == 1)
					printf("%s: THIS REQUEST WAS FOR ME!\n", my_ifaces[i].name);

				// So we answer !
				char *reply = buildArpReply(my_ifaces[i].ipAddress, my_ifaces[i].macAddress, ntohl(arp_spa), arp->arp_sha);
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

					if(ntohl(arp_dpa) == iface->ipAddress)
					{
						if(DEBUG == 1)
							printf("%s: THIS REPLY WAS FOR ME!\n", iface->name);

						// adds to list and wakes server thread
						Node *_newLine = newLine(ntohl(arp_spa), arp->arp_sha, currentTTL, iface->name);
						addLine(&arpTable, _newLine, DYNAMIC_ENTRY);

						// Just some extra careful
						if(waitingReply[iface->id] != 0)
						{
							sem_post(&serverSemaphore);
						}
					}
			}
    }
		// ARP
		//...
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
		if(n < 0) {
			fprintf(stderr, "ERROR: %s\n", strerror(errno));
			exit(1);
		}

		doProcess(packet_buffer, n, ifn);
	}
}
/* */

void loadIfces(int numIfaces, char **argv)
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
	}
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

void line2NetworkByteOrder(Node *line)
{
  line->ipAddress = htonl(line->ipAddress);
  line->ttl = htons(line->ttl);
}

void sendLines(int socket, Node* arpTable)
{
  Node *line = arpTable;
	Node aux;
  unsigned int lineLen = sizeof(Node);
  while (line->next != NULL)
  {
		aux = *(line->next);
    line2NetworkByteOrder(&aux);
    _send(socket, (char*) &aux, lineLen);
    line = line->next;
  }
}

void sendIface(int socket, MyInterface* iface)
{
	//converts ifaces atributes to network byte order
	iface2NetworkByteOrder(iface);
	_send(socket, (char*) iface, sizeof(MyInterface));
}

void safeIfaceCopy(MyInterface *dst, MyInterface *src, unsigned char ifaceIndex)
{
	const static unsigned char myInterfaceSize = sizeof(MyInterface);
	sem_wait(&ifaceMutexes[ifaceIndex]);
	memcpy(dst, src, myInterfaceSize);
	sem_post(&ifaceMutexes[ifaceIndex]);
}

void sendIfaces(int socket)
{
	MyInterface aux;
	for(int i = 0; i < numIfaces; i++)
	{
		safeIfaceCopy(&aux, &my_ifaces[i], i);
		sendIface(socket, &aux);
	}
}

unsigned char getIfaceIndex(const char *ifname)
{
	unsigned char i;
	for(i = 0; i < numIfaces; i++)
	{
		// paglijonson style
		if(strcmp(my_ifaces[i].name, ifname) == 0) break;
	}
	return i;
}

void configIface(const char *ifname, unsigned int ip, unsigned int mask)
{
	unsigned char i = getIfaceIndex(ifname);

	if(DEBUG == 1)
		printf("CONFIG INTERFACE IP: %u\n", ip);

	if(i < numIfaces) // iface found
	{
		sem_wait(&ifaceMutexes[i]);
		my_ifaces[i].ipAddress = ip;
		my_ifaces[i].netMask = mask;
		sem_post(&ifaceMutexes[i]);
	}
}

void setMTUSize(const char *ifname, unsigned short mtu)
{
	unsigned char i = getIfaceIndex(ifname);

	if(i < numIfaces)
	{
		if(DEBUG == 1)
			printf("%s MTU size: %u\n", ifname, mtu);

		sem_wait(&ifaceMutexes[i]);
		my_ifaces[i].mtu = mtu;
		sem_post(&ifaceMutexes[i]);
	}
}

void resolveIP(unsigned int ip, int socket)
{
	// first thing is to look for the ip at the arp table
	Node *line = searchLine(&arpTable, ip);

	if(line == NULL) // line not found
	{
		// searchs the ie in the same network of the requested ip address
		unsigned char i;
		unsigned int ifaceIP, ifaceNetmask;
		unsigned char ifaceMAC[6];
		for(i = 0; i < numIfaces; i++)
		{
			sem_wait(&ifaceMutexes[i]);
			ifaceIP = my_ifaces[i].ipAddress;
			ifaceNetmask = my_ifaces[i].netMask;
			memcpy(ifaceMAC, my_ifaces[i].macAddress, 6);
			sem_post(&ifaceMutexes[i]);

			if((ifaceIP & ifaceNetmask) == (ip & ifaceNetmask)) break;
		}

		if(i < numIfaces) // ie some iface's network matches with ip resquested network
		{
			waitingReply[i] = 1;

			char *request = buildArpRequest(ifaceIP, ifaceMAC, ip);
			sendArpPacket(request, &my_ifaces[i]);
			free(request);

			if(DEBUG == 1)
				printf("ARP REQUEST SENT BY %s\n", my_ifaces[i].name);

			// get time info
			clockid_t clockID = CLOCK_REALTIME; // this clock has the time in seconds
																					 // and nanoseconds since THE EPOCH
																					 // what happens when the representation ends ?? POW

			struct timespec ts; //  struct to stores the time
			clock_gettime(clockID, &ts);
			ts.tv_sec += ARP_TIMEOUT;

			// waits for an answer
			// locks the interface for receiving packets
			int ret = sem_timedwait(&serverSemaphore, &ts);
			waitingReply[i] = 0;

			if(ret != ETIMEDOUT) // ie: timeout was not exceeded
			{
				line = searchLine(&arpTable, ip);
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
		response[0] == __OK__;
		short int ttl = htons(line->next->ttl);
		memcpy(response+1, line->next->macAddress, 6);
		memcpy(response+7, &ttl, 2);
	}
	_send(socket, (char*) &response, 9);
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

			char ifName[MAX_IFNAME_LEN];
      unsigned int ip;
			short int _ttl;

			switch(opCode)
			{
				case LIST_IFCES: // lists all ifaces
					sendIfaces(newsockfd);
					if(DEBUG == 1)
						printf("IFACES SENT\n");
					break;

				case LIST_IFACE: // lists a single iface
					strcpy(ifName, message);
					n = getIfaceIndex(ifName);
					if(n < numIfaces)
					{
						MyInterface aux;
						safeIfaceCopy(&aux, &my_ifaces[n], n);
						sendIface(newsockfd, &aux);
					}
					break;

				case CONFIG_IFACE:
					// message decode
					strcpy(ifName, message);
					unsigned char nameLen = strlen(ifName);
					message += nameLen+1;
					ip = ntohl(*(unsigned int*)message);
					message += 4;
					unsigned int mask = ntohl(*(unsigned int*)message);
					configIface(ifName, ip, mask);
					break;

				case SET_IFACE_MTU:
					// message decode
					strcpy(ifName, message);
					unsigned char ifaceNameLen = strlen(ifName);
					unsigned short mtuSize = ntohs(* (unsigned short*)(message+ifaceNameLen+1));
					setMTUSize(ifName, mtuSize);
					break;

        case ADD_ARP_LINE:
          //adds a new line on arp table
          ip = ntohl(*(unsigned int*)message);
          message += 4 + 6;
          short int ttl = ntohs(*(short int*)message);
          Node *l = newLine(ip, message - 6, ttl, NULL);
          addLine(&arpTable, l, STATIC_ENTRY);
          break;

				case SHOW_TABLE:
          sendLines(newsockfd, &arpTable);
          break;

				case DEL_ARP_LINE:
					ip = ntohl(*(unsigned int*) message);
					removeLine(&arpTable, ip);
					break;

				case RES_IP:
					ip = ntohl(*(unsigned int*) message);
					resolveIP(ip, newsockfd);
					break;

				case SET_ARP_TTL:
					_ttl = ntohs(*(short int*) message);
					currentTTL = _ttl;
					break;

				default:
					printf("OPERATION NOT SUPPORTED BY XARPD\n");
			}
		}
		close(newsockfd);
	}
}

void decrementer()
{
  Node *line;
	Node *next;
  while(1)
	{
    sleep(1);
    line = arpTable.next;
    while (line != NULL)
    {
			next = line->next;

			if(line->ttl != -1)
				line->ttl--;

      if (line->ttl== 0)
      {
        removeLine(&arpTable, line->ipAddress);
      }

			line = next;
    }
  }
}

void initMutexes(int numSem)
{
	ifaceMutexes = (sem_t*) malloc(numSem * sizeof(sem_t));
	for(int i = 0; i < numSem; i++)
	{
		sem_init(&ifaceMutexes[i], 0, 1); // mutex compartilhado por todas as threads
	}
}

// main function
int main(int argc, char** argv)
{
	arpTable.next = NULL;
	sem_init(&(arpTable.semaphore), 0, 1);

	sem_init(&serverSemaphore, 0 , 0);

	if (argc < 2)
		print_usage();

	numIfaces = (argc-1 < MAX_IFACES+1) ? argc-1 : MAX_IFACES;

  pthread_t tid[numIfaces];
  pthread_t ttlDecrementer;

	my_ifaces = (MyInterface*) malloc(numIfaces * sizeof(MyInterface));
	memset(my_ifaces, 0, numIfaces * sizeof(MyInterface));

	waitingReply = (unsigned char *) malloc(numIfaces);
	memset(waitingReply, 0, numIfaces);

	initMutexes(numIfaces);
	loadIfces(numIfaces, argv);

	// This thread will be responsible for answer xarp and xifconfig demands
	pthread_create(&tid[argc - 1], NULL, (void*) server, NULL);
	pthread_create(&ttlDecrementer, NULL, (void*) decrementer, NULL);

	int i;
	for (i = 0; i < numIfaces; i++)
	{
		if(DEBUG == 1)
			print_eth_address(my_ifaces[i].name, my_ifaces[i].macAddress);

    int err = pthread_create(&(tid[i]), NULL, (void*) read_iface, (void *) &my_ifaces[i]);
		// Create one thread for each interface. Each thread should run the function read_iface.
	}

  for (i = 0; i < argc - 1; i++)
  {
    pthread_join(tid[i], NULL);
  }

	pthread_join(tid[argc-1], NULL);
  pthread_join(ttlDecrementer, NULL);

	return 0;
}
