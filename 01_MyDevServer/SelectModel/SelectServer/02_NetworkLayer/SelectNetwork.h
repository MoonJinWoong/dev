#pragma once
#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "NetErrSet.h"
#include "NetworkConfig.h"

namespace NetworkLib
{
	struct SocketInfo
	{
		SOCKET socket;
		bool isConnected= false;
		char buf[1024];
		int recvByte;
		int sendByte;
	};

	class SelectNetwork
	{
	public:
		SelectNetwork();
		~SelectNetwork();
		NET_ERR_SET Init();
		NET_ERR_SET InitSocket();
		NET_ERR_SET BindListen(short port, int backlogCount);
		void Run();
		void JoinClient();
		bool AddSocketInfo(SOCKET sock);
		void Process();
		int RecvPost(SOCKET &sock, char *buf);
		void RemoveSocketInfo(int idx);
	private:
		SOCKET m_ServerSockfd;

		FD_SET m_readSet, m_writeSet;
		SOCKET m_ClientSocketfd;

		int m_nSocketIdx;   // 소켓 개수도 가지고 있자

	private:
		ServerConfig m_Config;
		SocketInfo *m_pSocketInfo[FD_SETSIZE];
	protected:
	};
}