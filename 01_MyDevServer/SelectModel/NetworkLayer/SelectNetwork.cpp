#include <stdio.h>
#include <vector>
#include <deque>
#include <iostream>
#include "SelectNetwork.h"

namespace NetworkLib
{
	SelectNetwork::SelectNetwork() 
	{
		m_config.Port = 9000;
		m_config.BackLogCount = 32;
		m_config.MaxClientCount = 2000;
		m_config.ExtraClientCount = 64;

		m_config.MaxClientRecvBufferSize = 8192;
		m_config.MaxClientSendBufferSize = 8192;
	}

	 SelectNetwork::~SelectNetwork(){}
	 NET_ERROR_CODE SelectNetwork::Init()
	 {
		 // 1. socket 초기화
		 // 2. bind and listen select를위한 FDSET 세팅  
		 // 3. 세션 풀 생성 


		 auto ret = InitSocket();
		 if (ret != NET_ERROR_CODE::NONE) 
			 return ret;

		 NET_ERROR_CODE bindListenRet = BindAndListen(m_config.Port, m_config.BackLogCount);
		 if (bindListenRet != NET_ERROR_CODE::NONE)
			 return bindListenRet;
		 
		 FD_ZERO(&m_Readfds);
		 FD_SET(m_ServerSockfd, &m_Readfds);

		 auto SessionPoolSize =
			 CreateSessionPool(m_config.MaxClientCount + m_config.ExtraClientCount);
		 std::cout << "SessionPoolSize : " << SessionPoolSize << std::endl;
		 
		 return NET_ERROR_CODE::NONE;
	 }
	 NET_ERROR_CODE SelectNetwork::InitSocket()
	 {
		 WORD version = MAKEWORD(2, 2);
		 WSADATA wsaData;
		 WSAStartup(version, &wsaData);

		 m_ServerSockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		 if (m_ServerSockfd < 0)
			 return NET_ERROR_CODE::SERVER_SOCKET_CREATE_FAIL;
		 
		 auto n = 1;
		 if (setsockopt(m_ServerSockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
			 return  NET_ERROR_CODE::SERVER_SOCKET_SO_REUSEADDR_FAIL;
		 
		 return  NET_ERROR_CODE::NONE;
	 }
	 NET_ERROR_CODE SelectNetwork::BindAndListen(unsigned short port, int backlog)
	 {
		 SOCKADDR_IN server_addr;
		 ZeroMemory(&server_addr, sizeof(server_addr));
		 server_addr.sin_family = AF_INET;
		 server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		 server_addr.sin_port = htons(port);

		 if (bind(m_ServerSockfd, (SOCKADDR*)&server_addr, sizeof(server_addr)) < 0)
			 return NET_ERROR_CODE::SERVER_SOCKET_BIND_FAIL;
		 

		 unsigned long mode = 1;
		 if (ioctlsocket(m_ServerSockfd, FIONBIO, &mode) == SOCKET_ERROR)
			 return NET_ERROR_CODE::SERVER_SOCKET_FIONBIO_FAIL;

		 if (listen(m_ServerSockfd, backlog) == SOCKET_ERROR)
			 return NET_ERROR_CODE::SERVER_SOCKET_LISTEN_FAIL;

		 
		 return NET_ERROR_CODE::NONE;
	 }
	 int  SelectNetwork::CreateSessionPool(const int maxClientCount)
	 {
		 for (int i = 0; i < maxClientCount; ++i)
		 {
			 ClientSession session;
			 ZeroMemory(&session, sizeof(session));
			 session.Index = i;
			 session.pRecvBuffer = new char[m_config.MaxClientRecvBufferSize];
			 session.pSendBuffer = new char[m_config.MaxClientSendBufferSize];

			 m_ClientSessionPool.push_back(session);
			 m_ClientSessionPoolIndex.push_back(session.Index);
		 }

		 return maxClientCount;
	 }

}