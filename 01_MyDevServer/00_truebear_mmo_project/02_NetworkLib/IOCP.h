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

		void acceptThread();
		void workerThread();

	public:
		IOCP(Content* content);
		virtual ~IOCP();

		bool Start();
		void ShutDownServer();


		SOCKET getSocket();
		HANDLE getIocpHandle();
		void   runAccept(SOCKET acceptsock, SOCKADDR_IN addr);

	private:
		SOCKET m_listenSock;
		HANDLE m_hIocp;
		//std::thread * acceptthread;
		//std::vector<Thread*, 4> workerthread;

	

	};

}