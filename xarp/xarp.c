#include "../definitions.h"
#include "xarp.h"
#include "../communication.h"
#include "../linked_list.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close function

int buildCommunicationWithXARP()
{
  // Builds the essential to communicate with xarpd
  int socket;
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);
  return socket;
}

char getOperation(const char* c)
{
  if(strcmp("show", c) == 0) return SHOW_TABLE;
  else if(strcmp("res", c) == 0) return RES_IP;
  else if(strcmp("add", c) == 0) return ADD_LINE;
  else if(strcmp("del", c) == 0) return DEL_LINE;
  else if(strcmp("ttl", c) == 0) return SET_ARP_TTL;

  return __ERROR__;
}

void showArpTable()
{
  // Builds the essential to communicate with xarpd
  int socket;
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);

  char request = SHOW_TABLE;
  _send(socket, &request, 1);
  close(socket);

  int lineLen = sizeof(Node);
  char *buffer = (char*) malloc(lineLen);
  int n = 0;
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);
  int count = 0;
  printf("  Entrada  |   Endereço IP   | Endereço Ethernet | TTL\n");
  do
  {
    if(n == lineLen) n = 0; // clean n for next interation
    n += _recv(socket, buffer+n, lineLen-n);
    if(n == lineLen)
    {
      // printInterface((MyInterface*) buffer);
      Node *aux = (Node*) buffer;
      aux->ipAddress = ntohl(aux->ipAddress);
      aux->ttl = ntohs(aux->ttl);
      printLine(aux, count++);
      printf("\n");
    }
  } while(n);

  free(buffer);
  close(socket);

}

char addEntry(const char* ipAddr, const char* macAddress, const char* ttl)
{
  unsigned int ip = inet_addr(ipAddr); // converts from dot notation into binary
  short int ttlSize = htons(atoi(ttl));

  unsigned int _mac[6];
  sscanf(macAddress, "%x:%x:%x:%x:%x:%x", &_mac[0], &_mac[1], &_mac[2], &_mac[3], &_mac[4], &_mac[5]);
  unsigned char mac[6];
  for(int i = 0; i < 6; i++) mac[i] = _mac[i];

  // Prepares info to send
  unsigned char messageLen = 13;
  char message[messageLen];
  message[0] = ADD_LINE;
  memcpy(message+1, (char*)&ip, 4);
  memcpy(message+1+4, (char*)&mac, 6);
  memcpy(message+1+4+6, (char*)&ttlSize, 2);

  // Builds the essential to communicate with xarpd
  int socket;
  struct sockaddr_in serv_addr;
  loadSocketInfo(&serv_addr, LOOPBACK_IP, XARPD_PORT);
  makeNewSocketAndConnect(&socket, (struct sockaddr_in*) &serv_addr);

  _send(socket, message, messageLen);
  close(socket);

  return __OK__;
}

char setTTL(short int ttl)
{
  unsigned int ttlInNetworkByteorder = htons(ttl);

  // opCode ttl
  unsigned char messageLen = 1 + 2;
  char message[messageLen];
  message[0] = SET_ARP_TTL;
  memcpy(message+1, (char*) &ttlInNetworkByteorder, 2);

  int socket = buildCommunicationWithXARP();
  _send(socket, message, messageLen);
  close(socket);

  return __OK__;
}

char delEntry(const char *ipAddress)
{
  // Opcode ipAddress
  unsigned char messageLen = 1 + 4;
  char message[messageLen];

  message[0] = DEL_LINE;

  unsigned int ip = inet_addr(ipAddress);
  memcpy(message+1, (char*) &ip, 4);

  int socket = buildCommunicationWithXARP();
  _send(socket, message, messageLen);
  close(socket);

  return __OK__;
}

void resolveAddress(const char *ipAddress)
{
  // Opcode ipaddress
  unsigned char messageLen = 1 + 4;
  char message[messageLen];
  unsigned int ip = inet_addr(ipAddress);

  message[0] = RES_IP;
  memcpy(message+1, (char*) &ip, 4);

  int socket = buildCommunicationWithXARP();
  _send(socket, message, messageLen);
  close(socket);

  // status // mac // ttl
  unsigned char bufferSize = 1 + 6 + 2;
  char buffer[bufferSize]; // buffer to store the xarpd response
  socket = buildCommunicationWithXARP();

  int n = 0;
  do
  {
    n += _recv(socket, buffer+n, bufferSize-n);
  } while(n != bufferSize);
  close(socket);

  char status = buffer[0];
  if(status == __ERROR__)
  {
    printf("ADRESS NOT FOUND\n");
  }
  else
  {
    unsigned char *mac = (unsigned char*) (buffer+1);
    short int ttl = ntohs(*(short int*) (mac+6));
    printf("(%s, %X:%X:%X:%X:%X:%X, %d)\n", ipAddress, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ttl);
  }
}

// In the main will be implemented the parser
int main(int argc, char *argv[])
{
  // Naive check
  if(argc < 2 || argc > 5)
  {
    // print error
    exit(1);
  }

  char opCode = getOperation(argv[1]);
  char ret;
  switch(opCode)
  {
    case SHOW_TABLE:
      if(argc != 2)
      {
        // print error
        exit(1);
      }
      showArpTable();
      break;
    case RES_IP:
      if(argc != 3)
      {
        // print error
        exit(1);
      }
      resolveAddress(argv[2]);
      break;
    case ADD_LINE:
      if(argc != 5)
      {
        // print error
        exit(1);
      }
      ret = addEntry(argv[2], argv[3], argv[4]);
      if(ret == __OK__) printf("Input added correctly\n");
      else printf("Error when adding entry\n");
      break;
    case DEL_LINE:
      if(argc != 3)
      {
        // print error
        exit(1);
      }
      ret = delEntry(argv[2]);
      if(ret == __OK__) printf("Entry deleted sucessfully\n");
      else printf("There is no entry with this address");
      break;
    case SET_ARP_TTL:
      if(argc != 3)
      {
        // print error
        exit(1);
      }
      setTTL((short int) atoi(argv[2]));
      break;

    default:
      // print error
      exit(1);
  }
}
