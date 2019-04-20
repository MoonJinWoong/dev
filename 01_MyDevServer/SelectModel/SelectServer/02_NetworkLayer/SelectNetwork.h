#pragma once
#define FD_SETSIZE 5096
#include <vector>
#include <deque>


#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "Define.h"     // 클라 정보 구조체 담겨 있음 


namespace NetworkLayer
{
	class SelectNetwork
	{
	public:
		SelectNetwork();
		~SelectNetwork();
		void	InitNetwork();
		bool	BindAndListen();
		void	CreateClientPool(const int &maxClient);
		void	Run();
		void	AcceptNewClient();
		int		AllocClientIndex();
		void	RejectClient(SOCKET socket);
		void	ConnectedClient(const int client_index
			, const SOCKET whoSocket, const char* pIP);
	private:

		SOCKET m_ServerSocket;
		int64_t m_ConnectSeq = 0;
		FD_SET m_ReadSet, m_WriteSet;
		size_t m_ConnectedSessionCount = 0;
		std::vector<ClientsInfo> m_ClientsPool;
		std::deque<int> m_ClientPoolIndex;
	};
}