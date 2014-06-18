#ifndef __TCPSERVER_H
#define __TCPSERVER_H

#include "tcpBase.h"

#include <ctime>

class tcpServer : public tcpBase
{
protected:
	string connectedClient;

private:
	int iAddrLen, recvdatalen;
	bool serverStatus;
	
	SOCKADDR_IN addr, remote_addr;

public:
	tcpServer();
	virtual ~tcpServer();

	void StartServer(int p);
	void AcceptClient();
	void DisconnectClient();
	void ShutdownServer();

	void writeServerRecv();
	
	char* getConnectedClient();
	bool getServerStatus();
};

tcpServer::tcpServer() {}
tcpServer::~tcpServer() {}

void tcpServer::StartServer(int p)
{
	char szPort[6];
	itoa(p, szPort, 10);

	if (p < 0 || p > 65563) // handle error
	{
		setStatus("Invalid Port: " + (string)szPort );

		return;
	}

	if (serverStatus == TRUE)
	{
		setStatus("Already Connected\n");

		return;
	}

	serverStatus = FALSE;

	port = p;
    
	setStatus("Creating Socket...");
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR) // EXIT ON ERRORS - fix ~
	{
        setStatus("Error: " + (string)getErrorInfo() + "\n");
		exit(1);
	}
	setStatus("Ok\n");

	setStatus("Binding socket to port: " + (string)szPort + "...");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY); //listens on any interface
	ret = bind(sListen, (struct sockaddr *) &addr, sizeof(addr));
	exitOnErrors();
	setStatus("Ok\n");

	setStatus("Putting socket into listening mode...");
	ret = listen(sListen,10); //backlog 10
	exitOnErrors();
	setStatus("Ok\n");

	iAddrLen = sizeof(remote_addr);

	setStatus("Waiting for connections (Press Ctrl+C to exit)...\n");

	serverStatus = TRUE;
}

void tcpServer::AcceptClient()
{
	if (serverStatus == FALSE)
		return;
	
	sClient = accept(sListen, (struct sockaddr *) &remote_addr, &iAddrLen);
	
	while (sClient == SOCKET_ERROR)
	{
		setStatus("Error: " + (string)getErrorInfo() + "\n");

		closesocket(sClient);

		Sleep(2000);
		
		sClient = accept(sListen, (struct sockaddr *) &remote_addr, &iAddrLen);
	}
	
	connectedClient = inet_ntoa(remote_addr.sin_addr);
	setStatus(connectedClient + " connected\n");
}

void tcpServer::DisconnectClient()
{
	ret = closesocket(sClient);
	exitOnErrors();

	setStatus( connectedClient + " disconnected\n" + "Waiting for connections...\n\n");

	connectedClient = "null";
}

void tcpServer::ShutdownServer()
{
	if (serverStatus == TRUE)
	{
		setStatus("Shutting down server...");
		ret = closesocket(sListen);
		exitOnErrors();
		setStatus("Ok");

		serverStatus = FALSE;
	}
}

void tcpServer::writeServerRecv()
{
	string fileName;
	time_t t;
	
	time(&t);

	
	fileName += "logs\\" + connectedClient + " (";
	fileName += ctime(&t);
	
	fileName.erase( fileName.find("\n",0) , 1);

	while(fileName.find(":",0) != string::npos )
		fileName.replace( fileName.find(":",0) , 1 , "." );
	
	fileName += ").txt";

	

	writeData( recvdata, recvbytes, (char *)fileName.c_str());
}

bool tcpServer::getServerStatus()
{
	return serverStatus;
}

char* tcpServer::getConnectedClient()
{
	return (char *) connectedClient.c_str();
}

#endif