#include "tcpClient.h"

#include <queue>

class ircClient : public tcpClient
{
private:
	string server, nick;
	//vector<string> channels;
	queue<string> messages;

public:
	ircClient(char* server_arg, int port_arg, string nick_arg)
	{
		server = server_arg;
		port = port_arg;
		nick = nick_arg;
		
		Connect(server_arg, port);

		Nick(nick);
		User(nick, "irc.localhost.com", "google.com", "MickeyMouse");
		Join("#test");
		RecvData();
	}

	~ircClient()
	{
		if ( connected )
		{
			Quit("Bye!");
			FinishSending();
			Disconnect();
		}
	}

	void Nick(string nick_arg)
	{
		nick = nick_arg;

		sendIRCLine("NICK " + nick);
	}

	void User(string username, string hostname, string email, string name)
	{
		//USER username "website" "hostname" :Name
		//string sendStr = "USER " + username + " " + hostname + " :" + name + "\n";

		string sendStr = "USER " + username + " \"" + email + "\" \"" + server + "\" :" + name;
		sendIRCLine(sendStr);

		//sendIRCLine("USER lusr \"google.com\" \"localhost\" :MickeyMouse");
	}

	void Join(string channel)
	{
		sendIRCLine("JOIN " + channel);
	}

	void Part(string channel)
	{
		sendIRCLine("PART " + channel);
	}

	void Quit(string message)
	{
		sendIRCLine("QUIT :" + message);
	}

	void Pong(string daemon)
	{
		sendIRCLine("PONG :" + daemon);
	}

	void PrivMsg(string receiver, string message)
	{
		sendIRCLine("PRIVMSG " + receiver + " :" + message);
	}

	virtual void RunIRCClient()
	{
		ret = 1;
		
		if ( recvdata )
			delete [] recvdata;

		recvdata = new char[RECVSIZE];
string recvDataStr;
		while ( ret > 0 )
		{
			ret = recv( sClient, recvdata, RECVSIZE, 0);
			
			exitOnErrors();

			recvDataStr = recvdata;
			
			recvDataStr = recvDataStr.substr(0, ret);

			if ( ret == RECVSIZE )
			{
				while ( ret == RECVSIZE )
				{
					string tmp;

					ret = recv( sClient, recvdata, RECVSIZE, 0);
					
					tmp = recvdata;

					tmp = tmp.substr(0, ret);

					recvDataStr += tmp;
				}

			}

			cout << recvDataStr << endl;

			while ( recvDataStr.size() > 0 )
			{
				int pos = recvDataStr.find("\r\n");

				messages.push( recvDataStr.substr(0,pos) );
				
				recvDataStr = recvDataStr.substr(pos+2);
			}

			writeData(recvDataStr, "log.txt");

			while ( messages.size() > 0 )
			{
				processIRCLine( messages.front() );
				messages.pop();
			}
		}

	}

	void processIRCLine(string line)
	{
		if ( line.find("PING :") != string::npos )
			ProcessPing(line);
		//else if ();
	}

	void sendIRCLine(string s)
	{
		SendData(s + "\n");
	}

	void ProcessPing(string line)
	{
		string daemon = line;
		int posStart = line.find("PING :");
		int posEnd = line.find("\r\n", posStart);

		daemon = daemon.substr( posStart + strlen("PING :"), posEnd-posStart-2 );
		
		Pong(daemon);
		//PrivMsg("user-name", "PONG: " + daemon);
	}

	void ProcessPrivMsg(string line)
	{

	}
};