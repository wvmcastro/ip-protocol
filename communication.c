#include "communication.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void errorHandler(void)
{
  fprintf(stderr, "ERROR: %s\n", strerror(errno));
  exit(1);
}

int _socket(int domain, int type, int protocol)
{
  int sockfd = socket(domain, type, protocol);
  if(sockfd < 0)
  {
    errorHandler();
  }
  return sockfd;
}

void loadSocketInfo(struct sockaddr_in *serv_addr, const char *ipAddress, int port)
{
  memset((char*) serv_addr, 0, sizeof(*serv_addr));
  serv_addr->sin_family = AF_INET;
  serv_addr->sin_addr.s_addr = inet_addr(ipAddress);
  serv_addr->sin_port = port;
}


void _bind(int *sockfd, struct sockaddr *address)
{
  if(bind(*sockfd, address, sizeof(*address)) < 0)
  {
    errorHandler();
  }
}

void _listen(int sockfd, unsigned int queueSize)
{
  if(listen(sockfd, queueSize) < 0)
  {
    errorHandler();
  }
}


int _accept(int sockfd, struct sockaddr *cli_addr)
{
  unsigned int lenCliAddr = sizeof(*cli_addr);
  int newsockfd = accept(sockfd, cli_addr, &lenCliAddr);
  if(newsockfd < 0)
  {
    printf("lenCliAddr: %d\n",lenCliAddr);
    errorHandler();
  }

  return newsockfd;
}

int _recv(int socket, char *buffer, unsigned int bufferSize)
{
  memset(buffer, 0, bufferSize);

  int n = recv(socket, buffer, bufferSize, 0);
  if(n < 0)
  {
    errorHandler();
  }
  return n;
}

void _send(int newsockfd, char *message, unsigned int messageSize)
{
  int n = 0;
  do
  {
      n += send(newsockfd, message+n, messageSize - n, 0);
      if (n < 0)
      {
        errorHandler();
      }
  } while (n < messageSize);
}

void _connect(int socket, struct sockaddr_in* serv_addr, unsigned int sockSize)
{
  int ret = connect(socket, (struct sockaddr*) serv_addr, sockSize);
  if(ret < 0)
  {
    errorHandler();
  }
}

void sendPacket(int socket, const char *ipAddress, int port, char* packet, unsigned int packetSize)
{
    struct sockaddr_in serv_addr;
    loadSocketInfo(&serv_addr, ipAddress, port);
    _connect(socket, &serv_addr, sizeof(serv_addr));
    _send(socket, packet, packetSize);
}

void makeNewSocketAndConnect(int *socket, struct sockaddr_in* serv_addr)
{
  *socket = _socket(AF_INET, SOCK_STREAM, 0);
  _connect(*socket, serv_addr, sizeof(*serv_addr));
}
