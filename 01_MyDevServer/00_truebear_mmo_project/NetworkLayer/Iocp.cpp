

#include "Iocp.h"

namespace NetworkLayer
{
	Iocp::Iocp() {}
	Iocp::~Iocp() {}
	bool Iocp::InitNetwork()
	{
		WSADATA   wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);

		m_IocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

		m_ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		if (m_ListenSocket == INVALID_SOCKET)
		{
			std::cout << "WSA Socket() Create failed...!" << std::endl;
			return false;
		}
			

		SOCKADDR_IN ServerAddr;
		ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(SERVER_PORT);
		ServerAddr.sin_addr.s_addr = INADDR_ANY;

		auto ret = ::bind(m_ListenSocket, reinterpret_cast<sockaddr*>(&ServerAddr), sizeof(ServerAddr));
		if (ret == SOCKET_ERROR)
		{
			std::cout << "bind error...!" << std::endl;
			return false;
		}

		auto ret2 = listen(m_ListenSocket, SOMAXCONN);
		if (ret == SOCKET_ERROR)
		{
			std::cout << "listen error...!" << std::endl;
			return false;
		}


		std::cout << "Server Init Complete...." << std::endl;
		return true;
	}
}