#include "main.h"

int main(int argc, char** argv)
{
	httpClient hClient;

	hClient.Connect("www.google.com", 80);
	hClient.SendData("GET / HTTP/1.1\r\n\r\n");
	hClient.RecvData();

	cout << hClient.getRecvdData() << endl;

	return 0;
}