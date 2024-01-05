#ifndef GROUPCHATAPP_SOCKETUTIL_H
#define GROUPCHATAPP_SOCKETUTIL_H


struct sockaddr_in *createIpv4Address(const char *ip, int port);

int createTCPIPv4Socket();


#endif //GROUPCHATAPP_SOCKETUTIL_H

