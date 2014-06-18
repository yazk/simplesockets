#include "udpBase.h"

class udpClient : public udpBase
{
protected:

public:
	udpClient(char *h, const int p);
	virtual ~udpClient();
};

udpClient::udpClient(char *h, const int p)
{
	ZeroMemory(&sendToAddr, sizeof(sendToAddr));
	he = NULL;

	ResolveHost(h, p, sendToAddr, he);
}

udpClient::~udpClient()
{
}