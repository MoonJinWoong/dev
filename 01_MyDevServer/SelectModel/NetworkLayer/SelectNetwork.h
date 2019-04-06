#pragma once
#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>

#include <vector>
#include <deque>
#include <unordered_map>

#include "NetworkFrame.h"

namespace NetworkLib
{
	class SelectNetwork :public NetworkFrame
	{
	public:
		SelectNetwork();
		virtual ~SelectNetwork();
		virtual NET_ERROR_CODE Init();
		//NET_ERROR_CODE SendData(const int sessionIndex, const short packetId
		//	, const short size, const char* pMsg) override;

	protected:
		SOCKET m_ServerSockfd;
		ServerConfig m_config;
		fd_set m_Readfds;

		std::vector<ClientSession> m_ClientSessionPool;
		std::deque<int> m_ClientSessionPoolIndex;
	protected:
		NET_ERROR_CODE InitSocket();
		NET_ERROR_CODE BindAndListen(unsigned short port , int backlog);
		int CreateSessionPool(const int maxClientCount);

	};
}