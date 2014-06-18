#ifndef __UDP_BASE_H
#define __UDP_BASE_H

#include "SocketBase.h"

class udpBase : public SocketBase
{
protected:
	SOCKET sckt;
	sockaddr_in sendToAddr, recvFromAddr;
	sockaddr_in my_addr; //recvFrom
	hostent *he;

	char host[256];

public:
	udpBase();
	virtual ~udpBase();

	void SendTo( char *sendData);
	void RecvFrom( const int port);

	void changePort(const int port);
	void ReResolveHost(char *h, const int p);
	bool setRecvFromInfo(const int p);
};

udpBase::udpBase()
{
	he = NULL;
	sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

udpBase::~udpBase()
{
	closesocket(sckt);
}

void udpBase::SendTo(char *sendData)
{
	if (sckt == INVALID_SOCKET)
	{
		cout << "Socket Error: " << WSAGetLastError() << endl;
		closesocket(sckt);
		sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

	ret = sendto(sckt, sendData, (int)strlen(sendData), 0, (struct sockaddr *)&sendToAddr, sizeof(sockaddr));
	exitOnErrors(); //TODO: Fix
}

void udpBase::RecvFrom(const int p)
{
	//TODO: freaks out when udp port is being used, return error codes instead of void

    sockaddr_in their_addr;
    int addr_len, numbytes;
    char buf[256];

    if (sckt == INVALID_SOCKET)
	{
		cout << "Socket Error: " << WSAGetLastError() << endl;
		closesocket(sckt);
		sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

	if ( !setRecvFromInfo(p) )
		return;
	

    addr_len = sizeof(struct sockaddr);
	numbytes = recvfrom(sckt, buf, sizeof(buf), 0, (struct sockaddr *)&their_addr, &addr_len);

    if ( numbytes == SOCKET_ERROR)
	{
		cout << "recvfrom Error: " << WSAGetLastError() << endl;
		return;
	}

	cout << "packet from " << inet_ntoa(their_addr.sin_addr) << " contains (" << numbytes << " bytes):" << endl;
	
	for (int x=0; x < numbytes; x++)
		cout << buf[x];

	cout << endl;
}

bool udpBase::setRecvFromInfo(const int p)
{
	if ( htons(p) != my_addr.sin_port )
	{
		my_addr.sin_family = AF_INET;			/* host byte order */
		my_addr.sin_port = htons(p);			/* short, network byte order */
		my_addr.sin_addr.s_addr = INADDR_ANY;	/* auto-fill with my IP */
		//bzero(&(my_addr.sin_zero), 8);        /* zero the rest of the struct */
		memset(&(my_addr.sin_zero), '\0', 8);
		
		if ( bind(sckt, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == SOCKET_ERROR)
		{
			cout << "Bind Error: " << WSAGetLastError() << " " << GetLastError() << endl;
			return false;
		}
	}

	return true;
}

void udpBase::changePort(const int p)
{
	sendToAddr.sin_port = htons(p);
}

void udpBase::ReResolveHost(char *h, const int p)
{
	ZeroMemory(&sendToAddr, sizeof(sendToAddr));
	he = NULL;

	ResolveHost(h, p, sendToAddr, he);
}

#endif