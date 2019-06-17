#pragma once

#include "preCompile.h"

namespace Core
{
	class ServerFrame;
	class Singleton;
}

namespace Network
{
	class IOCP : public Core::ServerFrame, public Singleton<IOCP>
	{
	private:
		bool CreateSocket();
		static DWORD WINAPI acceptThread(LPVOID pServer);
		static DWORD WINAPI workerThread(LPVOID pServer);

	public:
		IOCP(Content* content);
		virtual ~IOCP();

		bool Start();

		SOCKET getSocket();
		HANDLE getIocpHandle();
		void   runAccept(SOCKET acceptsock, SOCKADDR_IN addr);

	private:
		SOCKET m_listenSock;
		HANDLE m_hIocp;
		Thread* acceptthread;
		std::array<Thread*, 4> workerthread;

	

	};
}