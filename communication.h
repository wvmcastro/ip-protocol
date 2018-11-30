#ifndef __COMMUNICATION__
#define __COMMUNICATION__

#include <netinet/in.h>

void errorHandler(void);

int _socket(int, int, int);

void loadSocketInfo(struct sockaddr_in*, const char *, int);

void _bind(int*, struct sockaddr*);

void _listen(int, unsigned int);

int _recv(int, char*, unsigned int);

void _send(int, char*, unsigned int);

void _connect(int, struct sockaddr_in*, unsigned int);

void sendPacket(int, const char *, int, char *, unsigned int);

int _accept(int, struct sockaddr*);

void makeNewSocketAndConnect(int *, struct sockaddr_in*);


#endif
