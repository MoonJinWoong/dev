


#include "SelectNetwork.h"



namespace NetworkLayer
{
	SelectNetwork::SelectNetwork() {}
	SelectNetwork::~SelectNetwork() {}
	bool SelectNetwork::Init()
	{
		// 1 - socket init 
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);

		m_ServerSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSockfd < 0)
		{
			std::cout << "socket create failed...!" << std::endl;
			return false;
		}
		auto n = 1;
		if (setsockopt(m_ServerSockfd, SOL_SOCKET, SO_REUSEADDR, (char*)& n, sizeof(n)) < 0)
		{
			std::cout << "sockOpt is failedd...!" << std::endl;
			return false;
		}


		// 2-  bind and listen
		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(9000);

		if (bind(m_ServerSockfd, (SOCKADDR*)& server_addr, sizeof(server_addr)) < 0)
		{
			std::cout << "bind is failed....!" << std::endl;
			return false;
		}

		unsigned long mode = 1;
		if (ioctlsocket(m_ServerSockfd, FIONBIO, &mode) == SOCKET_ERROR)
		{
			std::cout << "ioctIsocket is failed...!" << std::endl;
			return false;
		}

		if (listen(m_ServerSockfd, 32) == SOCKET_ERROR)
		{
			std::cout << "listen is failed...!" << std::endl;
			return false;
		}

		//  3 - fd init
		FD_ZERO(&m_Readfds);
		FD_SET(m_ServerSockfd, &m_Readfds);


		return true;
	}

	void SelectNetwork::Run()
	{
		auto read_set = m_Readfds;
		auto write_set = m_Readfds;

		timeval timeout{ 0, 1000 }; //tv_sec, tv_usec
		auto selectResult = select(0, &read_set, &write_set, 0, &timeout);

		// Accept
		if (FD_ISSET(m_ServerSockfd, &read_set))
			auto result = CheckNewClient();
	
		
	}

	bool SelectNetwork::CheckNewClient()
	{
		auto tryCount = 0; // 너무 많이 accept를 시도하지 않도록 한다.

		do
		{
			++tryCount;

			SOCKADDR_IN client_addr;
			auto client_len = static_cast<int>(sizeof(client_addr));
			auto client_sockfd = accept(m_ServerSockfd, (SOCKADDR*)& client_addr, &client_len);
			//m_pRefLogger->Write(LOG_TYPE::L_DEBUG, "%s | client_sockfd(%I64u)", __FUNCTION__, client_sockfd);
			if (client_sockfd == INVALID_SOCKET)
			{
				if (WSAGetLastError() == WSAEWOULDBLOCK)
				{
					std::cout << "accept Error..!" << std::endl;
					return false;
				}
			}

			char clientIP[MAX_IP_LEN] = { 0, };
			inet_ntop(AF_INET, &(client_addr.sin_addr), clientIP, MAX_IP_LEN - 1);

			SetSockOption(client_sockfd);

			FD_SET(client_sockfd, &m_Readfds);
			std::cout << "asdfasdf" << std::endl;

		} while (tryCount < FD_SETSIZE);
	}
	void SelectNetwork::SetSockOption(const SOCKET fd)
	{
		linger ling;
		ling.l_onoff = 0;
		ling.l_linger = 0;
		setsockopt(fd, SOL_SOCKET, SO_LINGER, (char*)& ling, sizeof(ling));

		//int size1 = m_Config.MaxClientSockOptRecvBufferSize;
		//int size2 = m_Config.MaxClientSockOptSendBufferSize;
		//setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)& size1, sizeof(size1));
		//setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)& size2, sizeof(size2));
	}



}