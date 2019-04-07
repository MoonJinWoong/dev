

#include "SelectNetwork.h"
namespace NetworkLib
{
	SelectNetwork::SelectNetwork() {}
	SelectNetwork::~SelectNetwork() {}
	NET_ERR_SET SelectNetwork::Init()
	{
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);

		m_ServerSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSockfd < 0)
		{
			return NET_ERR_SET::SERVER_SOCKET_CREATE_FAIL;
		}

		auto n = 1;
		if (setsockopt(m_ServerSockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
		{
			return NET_ERR_SET::SERVER_SOCKET_SO_REUSEADDR_FAIL;
		}

	}
}