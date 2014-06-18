#include "sample.h"

int main(int argc, char** argv)
{
	if (argv[argc-1][0] == '1')
	{
		tcpServer test;

		test.StartServer(7654);
	
		while (test.getServerStatus())
		{
			test.AcceptClient();
			test.SendData("Hello Client!");
			test.FinishSending();
			test.writeServerRecv();
			test.DisconnectClient();
		}
	}

	else
	{
		tcpClient test;

		test.Connect("192.168.1.2", 7654);
		test.RecvData();

		test.Disconnect();
	}

	Sleep(1000);
	return 0;
}

/*
	if (argv[argc-1][0] == '0') //httpClient - google
	{
		httpClient test;

		test.Connect("google.com", 80);
		//test.Connect("216.239.57.99", 80);

		test.SendData("GET / HTTP/1.0\r\n\r\n");
		test.FinishSending();
		test.RecvData();

		test.Disconnect();

		MessageBox(0, ((string)test.getStatus()).c_str(), "Status Messages", 0);
	}
	else if (argv[argc-1][0] == '1') //httpClient - yahoo weather
	{
		httpClient test;

		test.Connect("weather.yahoo.com", 80);

		test.SendData("GET /forecast/USFL0372_f.html HTTP/1.1\r\nHost: weather.yahoo.com\r\n\r\n");
		test.FinishSending();
		test.RecvData();

		test.Disconnect();
		
		test.writeData( test.getRecvdData(), test.getRecvdBytes(), "index.html");

		MessageBox(0, test.getStatus().c_str(), "Status Messages", 0);
	}
	else if (argv[argc-1][0] == '2') //tcpClient
	{
		tcpClient test;

		test.Connect("192.168.1.2", 7654);
		test.SendData("GET / HTTP/1.0\r\n\r\n");
		test.FinishSending();
		test.RecvData();

		test.Disconnect();

		MessageBox(0, test.getStatus().c_str(), "Status Messages", 0);
	}
	else if (argv[argc-1][0] == '3') //tcpServer
	{
		tcpServer test;

		test.StartServer(7654);
	
		while (test.getServerStatus())
		{
			test.AcceptClient();
			test.RecvData();
			test.SendData("Hello Client!");
			test.FinishSending();
			test.writeServerRecv();
			test.DisconnectClient();
		}
	}
	else if (argv[argc-1][0] == '4') //httpServer (http)
	{
		httpServer test(7654, HTTP_SERVER);
	}
	else if (argv[argc-1][0] == '5') //httpServer (wap)
	{
		httpServer test(7654, WAP_SERVER);
	}
	else if (argv[argc-1][0] == '6') //ircClient
	{
		ircClient test("203.167.224.18", 6667, "invibot", "#coderz");
	}
	else if (argv[argc-1][0] == '7') //udpClient
	{
		udpClient test("68.205.150.130", 7654);

		test.SendTo(lngstr); //test.changePort(rand()); //srand( time(NULL)*rand() );

		cout << "Data Sent";
	}
	else if (argv[argc-1][0] == '8') //udpClient
	{
		udpServer test;

		test.RecvFrom(7654);

		cout << "Data Recv";
	}
	else if (0) //udpClienttest
	{
		udpClient test("192.168.1.101", 80);

		for ( int x=0; x < 10; x++)
			test.SendTo(lngstr); //test.changePort(rand()); //srand( time(NULL)*rand() );

		cout << "Data Sent";
	}
	else
		MessageBox(0, "Invaild Selection", "Error", 0);
*/