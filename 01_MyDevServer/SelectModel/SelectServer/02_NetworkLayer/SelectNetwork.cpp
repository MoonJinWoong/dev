

#include "SelectNetwork.h"



namespace NetworkLayer
{
	SelectNetwork::SelectNetwork() {}
	SelectNetwork::~SelectNetwork() 
	{
		for (auto& client : m_ClientsPool)
		{
			if (client.pRecvBuffer) {
				delete[] client.pRecvBuffer;
			}

			if (client.pSendBuffer) {
				delete[] client.pSendBuffer;
			}
		}
	}
	void SelectNetwork::InitNetwork()
	{
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		 auto ret = WSAStartup(wVersionRequested, &wsaData);

		m_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSocket < 0) std::cout << "socket create failed.." << std::endl;

		auto n = 1;
		if (setsockopt(m_ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)& n, sizeof(n)) < 0)
			std::cout << "socket option set failed.." << std::endl;
			
		
		 auto isServerReady = BindAndListen();
		 if(isServerReady == false) std::cout << "BindAndListen failed.." << std::endl;


		 FD_ZERO(&m_ReadSet);
		 FD_SET(m_ServerSocket, &m_ReadSet);


		 // 각 클라마다 정보를 가지고 있을 수 있게 객체 생성
		 CreateClientPool(MAX_CLIENTS);

	}

	bool SelectNetwork::BindAndListen()
	{
		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(9000);

		if (bind(m_ServerSocket, (SOCKADDR*)& server_addr, sizeof(server_addr)) < 0)
		{ 
			std::cout << "bind is failed.." << std::endl;
			return false;
		}
		unsigned long mode = 1;
		if (ioctlsocket(m_ServerSocket, FIONBIO, &mode) == SOCKET_ERROR)
		{
			std::cout << "ioctIsocket is failed.." << std::endl;
			return false;
		}
		

		if (listen(m_ServerSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			std::cout << "listen is failed.." << std::endl;
			return false;
		}

			
		return true;
	}

	void SelectNetwork::CreateClientPool(const int &maxClient)
	{
		for (int i = 0; i < maxClient; ++i)
		{
			ClientsInfo client;
			ZeroMemory(&client, sizeof(client));
			client.Index = i;
			client.pRecvBuffer = new char[MaxClientRecvBufferSize];
			client.pSendBuffer = new char[MaxClientSendBufferSize];

			m_ClientsPool.push_back(client);
			m_ClientPoolIndex.push_back(client.Index);
		}
	}

	void SelectNetwork::Run()
	{
		// 소켓 셋 초기화
		auto rset = m_ReadSet;
		auto wset = m_ReadSet;

		timeval timeout{ 0, 1000 }; //tv_sec, tv_usec
		auto selectResult = select(0, &rset, &wset, 0, &timeout);
		if (selectResult < 0) std::cout << "select is faild...!"<< selectResult << std::endl;


		// Accept
		if (FD_ISSET(m_ServerSocket, &rset))
			AcceptNewClient();
		



	}

	void SelectNetwork::AcceptNewClient()
	{
		auto tryCount = 0; // 너무 많이 accept를 시도하지 않도록 한다.

		do
		{
			++tryCount;

			SOCKADDR_IN client_addr;
			auto client_len = static_cast<int>(sizeof(client_addr));
			auto client_sock = accept(m_ServerSocket, (SOCKADDR*)& client_addr, &client_len);
			//m_pRefLogger->Write(LOG_TYPE::L_DEBUG, "%s | client_sockfd(%I64u)", __FUNCTION__, client_sockfd);
			if (client_sock == INVALID_SOCKET)
				std::cout << "Accept error...!" << std::endl;
			


			// 새로 들어온 클라에 인덱스 번호 할당 
			auto newClientIndex = AllocClientIndex();


			if (newClientIndex < 0)
			{
				std::cout << "Client is full...!" << std::endl;
				// 더 이상 수용할 수 없으므로 바로 짜른다.
				RejectClient(client_sock);
			}


			char clientIP[MAX_IP_LEN] = { 0, };
			inet_ntop(AF_INET, &(client_addr.sin_addr), clientIP, MAX_IP_LEN - 1);

			linger ling;
			ling.l_onoff = 0;
			ling.l_linger = 0;
			setsockopt(client_sock, SOL_SOCKET, SO_LINGER, (char*)& ling, sizeof(ling));

			int size1 = MaxClientSockOptRecvBufferSize;
			int size2 = MaxClientSockOptSendBufferSize;

			setsockopt(client_sock, SOL_SOCKET, SO_RCVBUF, (char*)& size1, sizeof(size1));
			setsockopt(client_sock, SOL_SOCKET, SO_SNDBUF, (char*)& size2, sizeof(size2));

			FD_SET(client_sock, &m_ReadSet);
			//m_pRefLogger->Write(LOG_TYPE::L_DEBUG, "%s | client_sockfd(%I64u)", __FUNCTION__, client_sockfd);
			ConnectedClient(newClientIndex, client_sock, clientIP);

		} while (tryCount < 100);   // 100 대신에 FS_SETSIZE 해야함
	}


	int SelectNetwork::AllocClientIndex()
	{
		if (m_ClientPoolIndex.empty()) return -1;
		
		int index = m_ClientPoolIndex.front();
		m_ClientPoolIndex.pop_front();
		return index;
	}
	void SelectNetwork::RejectClient(SOCKET socket)
	{
		// 여기 구현해야함
	}

	void SelectNetwork::ConnectedClient(const int client_index
		, const SOCKET whoSocket, const char* pIP)
	{
		++m_ConnectSeq;

		auto& session = m_ClientsPool[client_index];
		session.unique_id = m_ConnectSeq;
		session.SocketFD = whoSocket;
		memcpy(session.IP, pIP, MAX_IP_LEN - 1);

		++m_ConnectedSessionCount;


		std::cout << "user id = " << client_index << std::endl;
	}
}