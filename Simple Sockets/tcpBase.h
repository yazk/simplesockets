#ifndef __TCP_BASE_H
#define __TCP_BASE_H

#include "SocketBase.h"

class tcpBase : public SocketBase
{
protected:
	int recvbytes;
	char *recvdata;
	ConnectionInfo *hostInfo;


	SOCKET sListen, sClient;

public:
	tcpBase();
	virtual ~tcpBase();

	virtual void RecvData();
	void SendData(string &sendData);
	void SendData(char *sendData);
	void SendFile(char filename[]);
	void FinishSending();

	int getRecvdBytes();
	char* getRecvdData();
	string getRecvdString();
};

tcpBase::tcpBase()
{
	recvbytes = 0;
	recvdata = NULL;
}

tcpBase::~tcpBase()
{
	if(recvdata)
		delete [] recvdata;
}

void tcpBase::RecvData()
{
	setStatus("Receiving Data...");

	if (recvdata)
		delete [] recvdata;

	recvdata = new char[RECVSIZE];
	vector <char> recvbuff;

	ret = recv( sClient, recvdata, RECVSIZE, 0);

	recvbytes = ret;
	
    if (ret == SOCKET_ERROR)
	{
		setStatus("Failed\n");
		exitOnErrors();
		return;
	}

	else if (ret != RECVSIZE) // cant do this on IRC
	{
		//cout.write(recvdata,ret);

		setStatus("Done");

		return;
	}
	
	while (ret > 0)
	{
		//cout.write(recvdata,ret);

		for (int x = 0; x < ret; x++)
			recvbuff.push_back(recvdata[x]); //recvbuff.insert( recvbuff.begin(), ret, recvdata[x]);

		ret = recv( sClient, recvdata, RECVSIZE, 0);
		
		//cout << "Ret: " << ret << endl;
	}

	setStatus("Done\n");

	recvbytes = (int)recvbuff.size();
	delete [] recvdata;
	recvdata = new char[recvbytes];
	
	for (int x=0; x < recvbytes; x++)
		recvdata[x] = recvbuff[x];

	setStatus("Ok");
}

void tcpBase::FinishSending()
{
	ret = shutdown(sClient,1);
	exitOnErrors();
}

void tcpBase::SendData(string &sendData)
{
	setStatus("Sending String...");

	ret = send(sClient, sendData.c_str(), (int)sendData.size(), 0);
	exitOnErrors();

	cout << "Sending String: " << sendData << endl;

	setStatus("Ok\n");
}

void tcpBase::SendData(char *sendData)
{
	setStatus("Sending Data...");

	ret = send(sClient, sendData, (int)strlen(sendData), 0); //TODO: fix SendData strlen (cant use with binary?)
	exitOnErrors();

	setStatus("Ok\n");
}

void tcpBase::SendFile(char filename[])
{
	setStatus("Sending File (" + (string)filename + ")...");

	if ( strlen(filename) == 0 )
	{
		setStatus("Failed (File Name Error)\n");
		return;
	}

	char sendData[RECVSIZE];
	int fileBytes, sentBytes = 0;
	ifstream in(filename, ios_base::binary);

	if (!in)
	{
		setStatus("Failed (File Error)\n");
		return;
	}
	
	in.seekg(0, ios::end);
	fileBytes = in.tellg();
	in.seekg(0, ios::beg);

	while (in)
	{
		in.read(sendData, RECVSIZE);
		ret = send(sClient, sendData, in.gcount(), 0);

		if (ret == SOCKET_ERROR)
		{
			in.close();

			setStatus("Failed (File Error)\n");

			return;
		}

		sentBytes += ret;

		cout << setprecision(3) << sentBytes/(double)fileBytes * 100  << "% sent  \r";
	}
	
	in.close();

	setStatus("Ok\n");

	return;
}

/*
void tcpBase::SendClass(type<t> *sendData)
{
	setStatus("Sending Data...");

	ret = send(sClient, sendData, (int)strlen(sendData), 0); //TODO: fix SendData strlen (cant use with binary?)
	exitOnErrors();

	setStatus("Ok\n");
}
*/

//// Data returning functions ////

int tcpBase::getRecvdBytes()
{
	return recvbytes;
}

char* tcpBase::getRecvdData()
{
	if (recvbytes > 0)
		return recvdata;
	else
		return "";
}

string tcpBase::getRecvdString()
{
	string rDataStr = recvdata;
	
	return rDataStr.substr(0, recvbytes);
}

#endif