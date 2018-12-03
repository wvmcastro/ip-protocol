#include <stdio.h>
#include <stdlib.h> // exit() malloc() free()
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // close function

#include "xroute.h"
#include "../definitions.h"
#include "../communication.h"
#include "../ip_linked_list.h"


char getOperation(const char *op)
{
  if(strcmp(op, "show") == 0) return SHOW_ROUTE_TABLE;
  if(strcmp(op, "add") == 0) return ADD_ROUTE_LINE;
  if(strcmp(op, "del") == 0) return DEL_ROUTE_LINE;

  return __ERROR__;
}

char addOrRemoveRouteLine(char opCode, const char *_target, const char *_netmask, const char *_gateway)
{
  unsigned int target = inet_addr(_target);
  unsigned int netmask = inet_addr(_netmask);
  unsigned int gateway = inet_addr(_gateway);

  // length opCode target netmask gateway
  unsigned char messageLen = 1 + 1 + 4 + 4 + 4;
  char message[messageLen];

  // building message packet
  message[0] = messageLen;
  message[1] = opCode;
  memcpy(message+2, (char*)&target, 4);
  memcpy(message+2+4, (char*)&netmask, 4);
  memcpy(message+2+4+4, (char*)&gateway, 4);

  // sends the message
  int socket = _socket(AF_INET, SOCK_STREAM, 0);
  sendPacket(socket, LOOPBACK_IP, XARPD_PORT, message, messageLen);
  close(socket);

  return __OK__;
}

char showRouteTable()
{
  char message[2];
  message[0] = 2;
  message[1] = SHOW_ROUTE_TABLE;

  // sends the message
  int socket = _socket(AF_INET, SOCK_STREAM, 0);
  sendPacket(socket, LOOPBACK_IP, XARPD_PORT, message, 2);

  int n = 0;
  unsigned char IPNodeLen = sizeof(IPNode);
  char *buffer = (char*) malloc(IPNodeLen);
  do
  {
    if(n == IPNodeLen)
    {
      // printa aqui
      n = 0;
    }
    n += _recv(socket, buffer, IPNodeLen);
  } while(n > 0);

  close(socket);
  return __OK__;
}

int main(int argc, char *argv[])
{
  char ret, opCode;

  if(argc < 2 || argc > 5 || (argc > 2 && argc < 5))
  {
    // bad input
    exit(1);
  }

  ret = __ERROR__;
  opCode = getOperation(argv[1]);
  switch(opCode)
  {
    case SHOW_ROUTE_TABLE:
      if(argc == 2) ret = showRouteTable();
      else exit(1);
      break;

    case ADD_ROUTE_LINE:
      if(argc == 5) ret = addOrRemoveRouteLine(ADD_ROUTE_LINE, argv[2], argv[3], argv[4]);
      else exit(1);
      break;

    case DEL_ROUTE_LINE:
      if(argc == 5) ret = addOrRemoveRouteLine(DEL_ROUTE_LINE, argv[2], argv[3], argv[4]);
      else exit(1);
      break;

    default:
      exit(1);
  }

  if(ret != __OK__)
  {
    printf("ERROR\n");
  }
}
