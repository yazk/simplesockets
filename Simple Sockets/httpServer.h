#include "tcpServer.h"

#define HTTP_SERVER 1
#define WAP_SERVER 2

class httpHeaderOLD
{
public:
	string method;
	string requestedFile;
	string header;
};

struct httpHeader
{
	string statusCode, connection, contentType, cacheControl, hostIpAndPort;
} header;

class httpServer : public tcpServer
{
private:
	//httpHeaderOLD header;

public:
	httpServer(const int p, const int typeOf);
	virtual ~httpServer();

	virtual void RecvData();

	void WAP_Server();
	void HTTP_Server();
	void sendIrcLogLines(const int linesToRead);
	void cleanIRCData(string &data);
	//void SendHeader(string statusCode, string connectionType, string contentType);
	void SendHeader();
	void interpretHeader(char *data); //TODO: make it more compatiable

	string getRequestedFileName();
	string getContentType(string &file);
	int getFileSize(string &file);
	string getFileFullPath(string file);
	
};

httpServer::httpServer(const int p, const int typeOf)
{
	port = p;

	if ( typeOf == HTTP_SERVER )
		HTTP_Server();
	else if ( typeOf == WAP_SERVER )
		WAP_Server();
	//else
		//MessageBox(0, "Wrong Server Option", "Error", 0);
}

httpServer::~httpServer() {}

//OLD
//void httpServer::SendHeader(string statusCode, string connectionType="close", string contentType="text/html")
//{
//	statusCode += " OK";
//
//	SendString("HTTP/1.1 " + statusCode + "\r\n");
//	SendString("Connection: " + connectionType + "\r\n");
//	SendString("Content-Type: " + contentType + "\r\n"); //text/plain text/html text/vnd.wap.wml
//
//	//SendData("Accept-Ranges: none\r\n");
//	//SendData("Host: 68.205.150.130:7654\r\n");
//
//	SendData("\r\n");
//}

void httpServer::SendHeader()
{
	int requestFileSize;
	char requestFileSizeSz[64];
	string requestFile, requestFileContentType;

	requestFile = getRequestedFileName();
	requestFileContentType = getContentType(requestFile);
	requestFileSize = getFileSize(requestFile);
	itoa(requestFileSize, requestFileSizeSz, 10);

	cout << "Client: " << getConnectedClient() << "\n"
		 << "Requests File: " << requestFile << " (" << requestFileSize << " bytes)" << "\n"
		 << "Content Type: " << requestFileContentType << endl;
	
	SendData((string)"HTTP/1.1 200\r\n");
	SendData((string)"Connection: close\r\n");
	if ( requestFileContentType.find("NULL") == string::npos)
		SendData("Content-Type: " + requestFileContentType + "\r\n");
	SendData("Cache-control: no-cache\r\n");
	if ( strcmp(requestFileSizeSz, "-1") )
		SendData("Content-Length: " + (string)requestFileSizeSz + "\n");
	SendData((string)"\r\n");
}

void httpServer::HTTP_Server()
{
	StartServer(port);

	while (getServerStatus())
	{
		AcceptClient();

		RecvData();

		//SendHeader("200", "close", "text/html");
		SendHeader();

		SendData("<html><body>Hello HTTP World!</body></html>");

		FinishSending();
		writeServerRecv();
		DisconnectClient();
	}

	ShutdownServer();
}

void httpServer::WAP_Server()
{
	StartServer(port);

	while (getServerStatus())
	{
		AcceptClient();

		RecvData();
		interpretHeader(recvdata);

		//SendHeader("200", "close", "text/vnd.wap.wml");
		SendHeader();

		sendIrcLogLines(10);
		FinishSending();

		writeServerRecv();
		DisconnectClient();
	}
}

//FIX !
void httpServer::interpretHeader(char *data)
{
	string header = data;

	string method = header.substr(header.find("GET"), header.find("\r\n") );
	
	int slashPos = (int)method.find("/");
	int spacePos = (int)method.find(" ", slashPos);

	string requestFile;

	if ( spacePos != string::npos && slashPos != string::npos )
		requestFile = method.substr( slashPos, spacePos - slashPos);

	if ( requestFile.size() > 0 && requestFile.find("sd=") != string::npos )
	{
		requestFile.erase(requestFile.find("/"),1);
		requestFile.erase(requestFile.find("sd="), 3);
		//TODO: send to irc
	}

}

string httpServer::getRequestedFileName()
{
	string rawData, method, requestFile;
	int slashPos, spacePos;

	//strncpy((char *)rawData.c_str(), recvdata, recvbytes-1); // why wont it work ? :/
	rawData = recvdata;
	
	method = rawData.substr(rawData.find("GET"), rawData.find("\r\n") );
	
	slashPos = (int)method.find("/"); //find
	spacePos = (int)method.find(" ", slashPos);

	if ( spacePos != string::npos && slashPos != string::npos )
		requestFile = method.substr( slashPos, spacePos - slashPos);

	if ( requestFile.size() < 1 )
		return "NULL";

	return requestFile;
}

string httpServer::getContentType(string &file) //TODO: test more - add more extensions
{
	string ext;
	int dotPos = (int)file.rfind(".");
	int endPos = (int)file.size();

	if ( dotPos != string::npos )
		ext = file.substr(dotPos, endPos-dotPos);
	else
		ext = "NULL";

	if (ext.find("html") != string::npos || ext.find("htm") != string::npos )
		return "text/html";
	else if (ext.find("jpg") != string::npos || ext.find("jpeg") != string::npos)
		return "image/jpeg";
	else if (ext.find("gif") != string::npos)
		return "image/gif";
	else if (ext.find("png") != string::npos)
		return "image/png";
	else if (ext.find("gcd") != string::npos)
		return "text/x-pcs-gcd";
	else if (ext.find("mid") != string::npos)
		return "audio/midi";
	else if (ext.find("txt") != string::npos)
		return "text/plain";
	else if (ext.find("qcd") != string::npos)
		return "audio/vnd.qcelp";
	else if (ext.find("pmd") != string::npos)
		return "application/x-pmd";
	else if (ext.find("jad") != string::npos)
		return "text/vnd.sun.j2me.app-descriptor";
	else if (ext.find("jar") != string::npos)
		return "application/java-archive";
	else
		return "NULL";
	
	//text/plain text/html text/vnd.wap.wml
}

string httpServer::getFileFullPath(string file)
{
	char currentDir[1024];

	GetCurrentDirectory(256, (LPTSTR)currentDir);

	if ( strstr(file.c_str(), currentDir) == NULL ) //( file.find(currentDir) == string::npos)
	{
		while ( file.find("/") != string::npos )
			file[file.find("/")] = '\\';

		if ( file[0] != '\\' )
			file = "\\" + file;

		return currentDir + file;
	}

	else
		return file;
}

int httpServer::getFileSize(string &file)
{
	string fileFullPath = getFileFullPath(file);
	//cout << "fileFullPath = " << fileFullPath << endl;

	HFILE hFile;
	OFSTRUCT of;
	
	of.cBytes = sizeof(of);
	of.fFixedDisk = 1;
	of.nErrCode = -1;
	strcpy( of.szPathName, fileFullPath.c_str());

	hFile = OpenFile((LPCSTR)fileFullPath.c_str(), (LPOFSTRUCT)&of, OF_READWRITE);

	return GetFileSize((HANDLE)hFile, NULL);
}

void httpServer::sendIrcLogLines(const int linesToRead)
{
	ifstream in("channel-log.txt");
	
	int fileLines;
	char temp[500];
	string logData;

	for (fileLines = 0; !in.eof(); fileLines++) in.ignore(500,'\n');

	in.clear();
	in.seekg(0, ios::beg); //ios::ate check the end of the file for a empty line

	for (int x=0; x < fileLines-linesToRead; x++)
		in.ignore(500,'\n');

	while (in)
	{
		in.getline(temp, 500);
		logData += temp;
		logData += "\n";
	}

	in.close();

	cleanIRCData(logData);

	SendFile("wml.wml");
	SendData(logData);
	SendData("</p></card></wml>");
}

void httpServer::cleanIRCData(string &data)
{
	//Replace
	while ( data.find("<") != string::npos)
		data.replace(data.find("<"), 1, "&lt;");

	while ( data.find(">") != string::npos)
		data.replace(data.find(">"), 1, "&gt;");

	while ( data.find("\n") != string::npos)
		data.replace(data.find("\n"), 1, "<br/>");

	//Erase
	while ( data.find("[") != string::npos)
		data.erase(data.find("["), 1);

	while ( data.find("]") != string::npos)
		data.erase(data.find("]"), 1);

	while ( data.find("@") != string::npos)
		data.erase(data.find("@"), 1);

	while ( data.find("+") != string::npos)
		data.erase(data.find("+"), 1);

	while ( data.find("PM") != string::npos)
		data.erase(data.find("PM")-3, 5);
	
	while ( data.find("AM") != string::npos)
		data.erase(data.find("AM")-3, 5);
}

void httpServer::RecvData()
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

	else if (ret != RECVSIZE)
	{
		setStatus("Done");
		return;
	}
	
	while (ret > 0)
	{
		for (int x = 0; x < ret; x++)
			recvbuff.push_back(recvdata[x]); //recvbuff.insert( recvbuff.begin(), ret, recvdata[x]);
		ret = recv( sClient, recvdata, RECVSIZE, 0);

		if ( strstr(recvdata, "\r\n\r\n") != NULL)
			break;
	}

	setStatus("Done\n");

	recvbytes = (int)recvbuff.size();
	delete [] recvdata;
	recvdata = new char[recvbytes];
	
	for (int x=0; x < recvbytes; x++)
		recvdata[x] = recvbuff[x];

	setStatus("Ok");
}