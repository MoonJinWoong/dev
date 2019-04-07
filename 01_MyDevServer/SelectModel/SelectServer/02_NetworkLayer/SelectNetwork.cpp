
#include "SelectNetwork.h"
namespace NetworkLib
{
	SelectNetwork::SelectNetwork() {}
	SelectNetwork::~SelectNetwork() {}
	NET_ERR_SET SelectNetwork::Init()
	{


		m_nSocketIdx = 0;

		// socket()
		auto ret = InitSocket();
		if (ret != NET_ERR_SET::NONE) return ret;

		// bind and listen
		ret = BindListen(m_Config.Port, m_Config.BackLogCount);
		if (ret != NET_ERR_SET::NONE) return ret;
		
		// 넌블로킹 socket으로 전환 
		u_long on = 1;
		int result = ioctlsocket(m_ServerSockfd, FIONBIO, &on);
		if (ret == NET_ERR_SET::SOCKET_ERR) return ret;


		std::cout << "Select Network Init" << std::endl;
		return NET_ERR_SET::NONE;
	}
	NET_ERR_SET SelectNetwork::InitSocket()
	{
		WORD wVersionRequested = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(wVersionRequested, &wsaData);

		m_ServerSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_ServerSockfd < 0)
			return NET_ERR_SET::SERVER_SOCKET_CREATE_FAIL;

		auto n = 1;
		if (setsockopt(m_ServerSockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
			return NET_ERR_SET::SERVER_SOCKET_SO_REUSEADDR_FAIL;

	}
	NET_ERR_SET SelectNetwork::BindListen(short port, int backlogCount)
	{
		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr.sin_port = htons(port);

		if (bind(m_ServerSockfd, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
			return NET_ERR_SET::SERVER_SOCKET_BIND_FAIL;
		

		unsigned long mode = 1;
		if (ioctlsocket(m_ServerSockfd, FIONBIO, &mode) == SOCKET_ERROR)
			return NET_ERR_SET::SERVER_SOCKET_FIONBIO_FAIL;
		

		if (listen(m_ServerSockfd, backlogCount) == SOCKET_ERROR)
			return NET_ERR_SET::SERVER_SOCKET_LISTEN_FAIL;

		
		return NET_ERR_SET::NONE;
	}

	void SelectNetwork::Run()
	{
		// 소켓셋 초기화 
		FD_ZERO(&m_readSet);
		FD_ZERO(&m_writeSet);
		FD_SET(m_ServerSockfd, &m_readSet);

		// 분류해서 SET에 집어넣자
		for (int i = 0; i < m_nSocketIdx; i++)
		{
			if (m_pSocketInfo[i]->recvByte > m_pSocketInfo[i]->sendByte)
				FD_SET(m_pSocketInfo[i]->socket, &m_writeSet);
			else
				FD_SET(m_pSocketInfo[i]->socket, &m_readSet);
		}

		// select 
		int ret = 0;
		//timeval timeout{ 0, 1000 }; //tv_sec, tv_usec
		ret = select(0, &m_readSet, &m_writeSet, NULL, NULL);
		if (ret == SOCKET_ERROR) 
			std::cout << "SELECT() Error...!  in Run()" << std::endl;
	
		// 소켓셋 검사 첫번째 : Accept
		if (FD_ISSET(m_ServerSockfd, &m_readSet))
			JoinClient();

		// 소켓셋 검사 두번째 : 통신 
		Process();
	}

	void SelectNetwork::JoinClient()
	{

			SOCKADDR_IN client_addr;
			auto client_len = static_cast<int>(sizeof(client_addr));
			auto client_sockfd = accept(m_ServerSockfd, (SOCKADDR*)&client_addr, &client_len);
		
			if (client_sockfd == INVALID_SOCKET)
				std::cout << "invalid accept ...!" << std::endl;
			else
			{
				std::cout << "[client Join!!] " <<
					"  IP:"<<inet_ntoa(client_addr.sin_addr) <<
					"  PORT:"<< ntohs(client_addr.sin_port) << std::endl;

				// Socket 정보에 ADD 해준다.
				if (AddSocketInfo(client_sockfd) == false)
					std::cout << "Add socket is failed...!" << std::endl;
			}
	}

	bool SelectNetwork::AddSocketInfo(SOCKET sock)
	{
		if (m_nSocketIdx >= FD_SETSIZE)
		{
			std::cout << "socket count overflow...!" << std::endl;
			return false;
		}

		SocketInfo * pSocketInfo = new SocketInfo;
		if (pSocketInfo == nullptr) 
		{
			std::cout << "socketInfo Alloc Max..!" << std::endl;
			return false;
		}

		pSocketInfo->socket = sock;
		pSocketInfo->recvByte = 0;
		pSocketInfo->sendByte = 0;
		pSocketInfo->isConnected = true;

		m_pSocketInfo[m_nSocketIdx++] = pSocketInfo;

		return true;
	}

	void SelectNetwork::Process()
	{
		for (int i = 0; i < m_nSocketIdx; i++)
		{
			SocketInfo *pSockInfo = m_pSocketInfo[i];

			if (FD_ISSET(pSockInfo->socket, &m_readSet))
			{
				int recvVal = 0;
			    recvVal = recv(pSockInfo->socket, pSockInfo->buf,512,0);
				if (recvVal == SOCKET_ERROR || recvVal == 0)
				{
					RemoveSocketInfo(i);
					continue;
				}
				pSockInfo->recvByte = recvVal;
				std::cout << "데이터 받음" << std::endl;
			}


			if (FD_ISSET(pSockInfo->socket, &m_writeSet))
			{
			
				int sendVal = 0;
				sendVal = send(pSockInfo->socket
					, pSockInfo->buf + pSockInfo->sendByte
					, pSockInfo->recvByte - pSockInfo->sendByte, 0);
				if (sendVal == SOCKET_ERROR || sendVal == 0)
				{
					RemoveSocketInfo(i);
					continue;
				}
			
				pSockInfo->sendByte += sendVal;
				if (pSockInfo->recvByte == pSockInfo->sendByte)
					pSockInfo->recvByte = pSockInfo->sendByte = 0;
			
			}
		}
	}
	void SelectNetwork::RemoveSocketInfo(int idx)
	{
		SocketInfo *pSocketInfo = m_pSocketInfo[idx];
		SOCKADDR_IN clientaddr;
		int addrLen = sizeof(clientaddr);
		getpeername(pSocketInfo->socket, (SOCKADDR *)&clientaddr, &addrLen);
		
		std::cout << "[" << idx << "]Client Close..!" << std::endl;
		
		
		closesocket(pSocketInfo->socket);
		delete pSocketInfo;

		// idx 땡겨주기 - 중간에 빵꾸나면 서칭할때 안된다 
		// 추후 개선 필요 
		if (idx != (m_nSocketIdx - 1))
			m_pSocketInfo[idx] = m_pSocketInfo[m_nSocketIdx - 1];

		--m_nSocketIdx;
	}
	int SelectNetwork::RecvPost(SOCKET &sock, char *buf)
	{
		// 추후 구현해야함
		return 1;
	}


}