// Simple Sockets
// TODO: fix connection info struct (make private not public) -> same with any other structs

#ifndef __SOCKET_BASE_H
#define __SOCKET_BASE_H

#pragma comment (lib, "wsock32")

#include <winsock.h> //wsock32.lib
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

#define RECVSIZE 1024

using namespace std;

struct ConnectionInfo
{
	char ipAddr[16], hostName[256];
	//char szPort[6];
	//int port;
} connectionInfo, myConnectionInfo;

class SocketBase
{
protected:
	int port, ret;
	string status;
	WSADATA wsda;

	bool bMyIp;

public:
	SocketBase();
	virtual ~SocketBase();

	void setStatus(string statusLine);
	string getStatus();
	bool writeData(char *data, int bytes, char filename[]);
	bool writeData(string data, char filename[]);
	string repeatChar(char repeatChar, int repeatCount);
	void exitOnErrors(char *additionalInfo);
	char* getErrorInfo();
	int ResolveHost(char *pHost, int pPort, SOCKADDR_IN &addr, hostent *host);
	string getMyIp();
};

SocketBase::SocketBase()
{
	if (WSAStartup(MAKEWORD(1,1), &wsda) == SOCKET_ERROR)
		exitOnErrors("WSAStartup");

	bMyIp = FALSE;
}

SocketBase::~SocketBase()
{
	WSACleanup();
}

int SocketBase::ResolveHost(char *pHost, int pPort, SOCKADDR_IN &addr, hostent *host)
//TODO: when i pass www. before the domain name the function sets connectionInfo wrong (works fine without www.)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(pPort);
	addr.sin_addr.s_addr = inet_addr(pHost);

	if ( addr.sin_addr.s_addr == INADDR_NONE) // have host name but not ip
	{
		setStatus("Resolving \"" + (string)pHost + "\"...");

		host = gethostbyname(pHost);

		if (host == NULL)
		{
			setStatus("Error: Unknown host: " + (string)pHost + "\n");

			strcpy(connectionInfo.ipAddr, "null");
			strcpy(connectionInfo.hostName, "null");

			return -1;
		}

		else
		{
			setStatus("Done\n");

			memcpy(&addr.sin_addr, host->h_addr, host->h_length);

			strcpy(connectionInfo.ipAddr, inet_ntoa(*((struct in_addr *)host->h_addr)) );
			strcpy(connectionInfo.hostName, host->h_name);
		}

	}

	else // have ip but not host name
	{
		host = gethostbyaddr(pHost, 4, AF_INET);

		if (host == NULL)
		{
			setStatus("Error: Cannot Resolve Host: " + (string)pHost + "\n");

			strcpy(connectionInfo.hostName, "null");
			strcpy(connectionInfo.ipAddr, pHost);
		}

		else
		{
			setStatus("Done\n");

			strcpy(connectionInfo.hostName, host->h_name);
			strcpy(connectionInfo.ipAddr, pHost);
		}
	}

	return 0;
}

string SocketBase::getMyIp()
{
	char hostdata[256];
	hostent *myHostName;

	if (!bMyIp)
	{
		strcpy(myConnectionInfo.ipAddr, "NULL");
		strcpy(myConnectionInfo.hostName, "NULL");

		if ( gethostname(hostdata, sizeof(hostdata)) == -1 ) // if can't resolve name
			return myConnectionInfo.ipAddr;

		strcpy(myConnectionInfo.hostName, hostdata);

		myHostName = gethostbyname(hostdata);
		
		if (!myHostName) // if ip resolve by hostname fails
			return myConnectionInfo.ipAddr;
		
		//if sucess
		bMyIp = TRUE;
		strcpy(myConnectionInfo.ipAddr, inet_ntoa(*((struct in_addr *)myHostName->h_addr)) );
	}

	return myConnectionInfo.ipAddr;
}

///////// Utility Functions /////////

bool SocketBase::writeData(char *data, int bytes, char filename[])
{
	if (bytes < 1)
		return FALSE;

	ofstream out(filename, ios_base::binary);

	if (!out || out.fail() || data == NULL)
		return FALSE;

	out.write(data, bytes);

	out.close();

	return TRUE;
}

bool SocketBase::writeData(string data, char filename[])
{
	if (data.size() < 1 ) //data==NULL
		return FALSE;

	ofstream out(filename);

	if ( !out || out.fail() )
		return FALSE;

	out.write(data.c_str(), data.size());

	out.close();

	return TRUE;
}

string SocketBase::repeatChar(char repeatChar, int repeatCount)
{
	string repeatStr = "";

	if (repeatCount < 0)
		return repeatStr;

	for (int x=0; x < repeatCount; x++)
		repeatStr += repeatChar;

	return repeatStr;
}

//////// Status Functions ////////

void SocketBase::setStatus(string statusLine)
{
	status += statusLine;
}

string SocketBase::getStatus()
{
	return status;
}

///////// Error Checking Functions /////////

void SocketBase::exitOnErrors(char *additionalInfo = "") // change this so it only closes the socket, and not the program.
{
    if (ret == SOCKET_ERROR)
	{
		if (additionalInfo == NULL) 
			setStatus("Error: " + (string)getErrorInfo() + "\n");
		else 
			setStatus((string)additionalInfo + " Error: " + getErrorInfo() + "\n");

		MessageBox(0, (char *)status.c_str(), "Status Messages", 0);

        //Sleep(5000);
		
		exit(WSAGetLastError());
	}
}

char* SocketBase::getErrorInfo()
{
	switch(WSAGetLastError())
	{
	
	case WSAECONNREFUSED:
		return "(10061) Connection refused";
		break;

	case WSAECONNRESET:
		return "(10054) Connection reset by peer";
		break;

	case WSAECONNABORTED:
		return "(10053) Software caused connection abort";
		break;

	case WSAETIMEDOUT:
		return "(10060) Connection timed out";
		break;

	case WSAEADDRINUSE:
		return "(10048) Address already in use";
		break;

	case WSAEDESTADDRREQ:
		return "(10039) Destination address required";
		break;
			
	case WSAEHOSTUNREACH:
		return "(10065) No route to host";
		break;

	case WSAEMFILE:
		return "(10024) Too many open files";
		break;

	case WSAENETDOWN:
		return "(10050) Network is down";
		break;

	case WSAENETRESET:
		return "(10052) Network dropped connection";
		break;

	case WSAENOBUFS:
		return "(10055) No buffer space available";
		break;

	case WSAENETUNREACH:
		return "(10051) Network is unreachable";
		break;

	case WSAHOST_NOT_FOUND:
		return "(11001) Host not found";
		break;

	case WSASYSNOTREADY:
		return "(10091) Network sub-system is unavailable";
		break;

	case WSANOTINITIALISED:
		return "(10093) WSAStartup() not performed";
		break;

	case WSANO_DATA:
		return "(11004) Valid name, no data of that type";
		break;

	case WSANO_RECOVERY:
		return "(11003) Non-recoverable query error";
		break;

	case WSATRY_AGAIN:
		return "(11002) Non-authoritative host found";
		break;

	case WSAVERNOTSUPPORTED:
		return "(10092) Wrong WinSock DLL version";
		break;

	case WSAEADDRNOTAVAIL:
		return "(10049) Can't assign requested address";
		break;

	case WSAEWOULDBLOCK:
		return "(10035) Resource temporarily unavailable";
		break;

	case WSAEISCONN:
		return "(10056) Socket is already connected";
		break;

	case WSAENOTCONN:
		return "(10057) Socket is not connected";
		break;

	case WSAESHUTDOWN:
		return "(10058) Can't send after socket shutdown";
		break;

	default:
		return "Unknown";
		break;
	}
}

#endif