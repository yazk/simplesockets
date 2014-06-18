#ifndef __TCPCLIENT_H
#define __TCPCLIENT_H

#include "tcpBase.h"

class tcpClient : public tcpBase
{
protected:
	bool connected;
	//string ipAddress, hostName;
	int port;

public:
	tcpClient();
	virtual ~tcpClient();

	int Connect(char *h, int p);
	void Disconnect();

};

tcpClient::tcpClient()
{
	connected = FALSE;
}

/*
tcpClient::tcpClient(char *pHost, int pPort)
*/

tcpClient::~tcpClient() {}

int tcpClient::Connect(char *pHost, int pPort)
{
	char szPort[6];
	itoa(pPort, szPort, 10);

	if (connected)
	{
		setStatus("Already Connected\n");

		return FALSE;
	}

	if (pPort < 0 || pPort > 65563)
	{
		setStatus("Invalid Port: " + (string)szPort );

		return FALSE;
	}

	SOCKADDR_IN addr;
	struct hostent *host;
	
	setStatus("Creating socket...");
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	while (sClient == SOCKET_ERROR) //TODO: can i do this? //i guess if sClient is a socket or did i miss the point of my question ~_~
	{
		setStatus("Error: " + (string)getErrorInfo() + "\n");

		Sleep(5000);

		sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_IP); // can i do this????
	}

	setStatus("Done\n");

	host = NULL;
	
	ret = ResolveHost(pHost, pPort, addr, host);
	exitOnErrors(); //TODO: fix

	port = pPort;

	setStatus("Connecting to " + (string)connectionInfo.hostName + " (" + (string)connectionInfo.ipAddr + ":" + (string)szPort + ")...");

	ret = connect(sClient, (struct sockaddr *) &addr, sizeof(addr));
	if (ret == SOCKET_ERROR) // || INVALID_SOCKET
	{
		setStatus("Failed\n"); // fire wall'd 

		port = -1;
		strcpy( connectionInfo.ipAddr, "null");  //connectionInfo.ipAddr = "null";
		strcpy( connectionInfo.hostName,"null"); //connectionInfo.hostName = "null";
		connected = FALSE;

		return FALSE;
	}

	else
		setStatus("Done\n");

    connected = TRUE;

	return TRUE;
}

void tcpClient::Disconnect()
{
	// note if closesocket is called more than once -1 10038 will be the return ( normally it would be 0)
	// WSAENOTSOCK (10038) Socket operation on non-socket.

	if (connected)
	{
		ret = closesocket(sClient);
		connected = FALSE;
	}

	else
		setStatus("Already Disconnected\n");

}

#endif