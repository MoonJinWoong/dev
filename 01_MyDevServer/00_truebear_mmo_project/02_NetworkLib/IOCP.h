#pragma once



#include "ServerFrame.h"


namespace Core
{
	class ServerFrame;
}

namespace NetworkLayer
{

	class Session;
	class SessionMgr;

	class IOCP : public Core::ServerFrame
	{

	public:
		IOCP();
		virtual ~IOCP();

		bool InitIocpServer();
		bool createThreads();
		bool startServer();
		void closeServer();


		SOCKET getListenSocket() const;
		HANDLE getIocpHandle() const;


		void ProcAccept(SOCKET sock, SOCKADDR_IN addrInfo);
	
	private:

		static DWORD WINAPI acceptThread(LPVOID server);
		static DWORD WINAPI workerThread(LPVOID server);

	private:
		SOCKET m_listenSocket;
		HANDLE m_hIocp;

		SessionMgr m_sessionMgr;
		
	};
}