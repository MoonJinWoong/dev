
#include "SelectNetwork.h"

namespace NetworkLib
{
	SelectNetwork::SelectNetwork() { }
	SelectNetwork::~SelectNetwork() {}

	bool SelectNetwork::Init(const ServerOption &option)
	{


		if (!InitSocket()) 
			std::cout << "InitSocket() is failed...!" << std::endl;
		return true;
	}
	void SelectNetwork::Start() {}
	void SelectNetwork::Stop() {}

	bool SelectNetwork::InitSocket()
	{
		WORD version = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(version, &wsaData);

		m_ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSock < 0)
		{
			std::cout << "socket call failed...!" << std::endl;
			return false;
		}

		auto n = 1;
		if (setsockopt(m_ServerSock, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
		{
			std::cout << "setsockopt is failed...!" << std::endl;
			return false;
		}

		std::cout << "Init Socket is Success...!" << std::endl;
		return true;
	}
}