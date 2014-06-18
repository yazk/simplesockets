#include "tcpClient.h"

#include <algorithm>

class httpClient : public tcpClient
{
private:
	string header;
	int downloadSize;
	vector<string> images;

public:
	httpClient();
	virtual ~httpClient();

	bool makeRequest(string method, string requestFile, string &request);
	void getLinks();
	void downloadLinks(string defaultHost, int port); //dev
	//void RecvData(); // Average Speed = (recvbytes/1024.0)/((downloadTime)/1000.0)

	bool removeHeader();
	void downloadBar();
	char* getHeader();
};

httpClient::httpClient() {}

httpClient::~httpClient() {}

/*
Status Code Range | Type of Code
---------------------------------
100				  | Informational
200				  | Successful
300				  | Redirection
400				  | Client Error
500				  | Server Error
*/

bool httpClient::makeRequest(string method, string requestFile, string& request)
{
	if (connected == FALSE)
		return FALSE;

	request = method + " ";
	request += requestFile + " ";
	//request += "\r\n";

	if ( strcmp( connectionInfo.hostName, "null" ) != 0)
	{
		request += "HTTP/1.1\r\nHost: ";
		request += /*(string)"www." +*/ connectionInfo.hostName;

		//cout << "\n\nCONN INFO: " << connectionInfo.hostName << "\n" << connectionInfo.ipAddr << "\n\n]]"; //doesnt work right when www. is passed with domain name
	}

	else
	{
		request += "HTTP/1.0";
	}
	
	request += "\r\n\r\n";
    
	return TRUE;
}

void httpClient::getLinks()
{
	string html = recvdata;
	string temp;

	transform(html.begin(),html.end(),html.begin(),tolower);

	for (int i = (int)html.find("<img src=",0); i != string::npos; i = (int)html.find("<img src=",i+1))
	{
		temp = html.substr(i+9, html.find(" ",i+5) - i - 9);
		
		while (temp.find("\"") != string::npos)
			temp.erase(temp.find("\""),1);

		while (temp.find("http://") != string::npos)
			temp.erase(temp.find("http://"),7);
		
		images.push_back(temp);
	}

}

void httpClient::downloadLinks(string defaultHost, int port) // dev
{
	string requestFile;
	string fileName;
	string host;

	for (int x=0; x < (int)images.size(); x++)
	{
		fileName = host = images[x];

		host = host.substr(0, host.find("/"));

		if (host.find(".") == string::npos)
			host = defaultHost;

		//if (host.find("www.") == string::npos) //////// fix meee
		//	host = "www." + host;

		fileName = fileName.substr(fileName.find("/"), fileName.size());

		requestFile = "GET ";
		requestFile += fileName;
		requestFile += " HTTP/1.1\r\nHost: ";
		requestFile += host;
		requestFile += "\r\n\r\n";

		fileName = fileName.erase(0, fileName.rfind("/")+1);
		//while (fileName.find("/") != string::npos)
		//	fileName.replace(fileName.find("/"),1,"\\");

		//fileName.erase(0,1);/////////

		//Disconnect();
		Connect((char *)host.c_str(), port);
		SendData((char *)requestFile.c_str());
		RecvData();

		if ( writeData( recvdata, recvbytes, (char *)fileName.c_str() ) )
			cout << "\nFile Outputted" << endl;

		cout << requestFile << endl;
	}
}

void httpClient::downloadBar()
{
	if (downloadSize > 0)
	{
		int barsDone = (int)(recvbytes/(double)downloadSize * 10);

		cout << "[" << repeatChar('|', barsDone ) << repeatChar('-', 10 - barsDone ) << "] "
			 <<  recvbytes/(double)downloadSize * 100 << "% completed " << recvbytes << "/" << downloadSize << "\r";
	}

	else if (downloadSize == 0)
	{
		string contentLength;

		int i;

		if ( header.find("Content-length: ") != string::npos )
			i = (int)header.find("Content-length: ");

		else if ( header.find("Content-Length: ") != string::npos )
			i = (int)header.find("Content-Length: ");

		else
		{
			downloadSize = -1;
			return;
		}

		if (header.find("\r\n",i) != string::npos)
			contentLength = header.substr(i, header.find("\r\n",i) - i );
		
		else if (header.find("\r",i) != string::npos)
			contentLength = header.substr(i, header.find("\r",i) - i );

		else if (header.find("\n",i) != string::npos)
			contentLength = header.substr(i, header.find("\n",i) - i );
		
		else
		{
			downloadSize = -1;
			return;
		}

		contentLength.erase(0,contentLength.find(" "));
		downloadSize = atoi(contentLength.c_str());
	}

	else
	{
		return;
	}
}

bool httpClient::removeHeader()
{
	char *temp;
	int tempSize;
	bool headerRead = FALSE;
	
	header = recvdata;

	for (int i = (int)header.find("\r\n",0); i != string::npos; i=(int)header.find("\r\n",i+1))
	{
		if (header[i+2] == '\n' || header[i+2] == '\r')
		{
			header.erase(i+4,header.size());
			headerRead = TRUE;
			break;
		}
	}

	if (!headerRead)
		return FALSE;

	tempSize = recvbytes - (int)header.size();
	temp = new char[tempSize];

	for(int x = 0; x+(int)header.size() < recvbytes; x++)
		temp[x] = recvdata[x+header.size()];

	delete [] recvdata;
	recvbytes = tempSize;
	recvdata = new char[recvbytes];
	memcpy(recvdata,temp,recvbytes);

	return TRUE;
}

char* httpClient::getHeader()
{
	return (char *)header.c_str();
}
