#pragma once

#include "ServerFrame.h"
namespace NetLib
{
	class IOCP : public ServerFrame
	{
	public:
		IOCP();
		virtual ~IOCP();

		bool InitIocpServer();
		bool createThreads();
		bool startServer();
		void closeServer();


		SOCKET getListenSocket();
		HANDLE getIocpHandle();
		void onAccept(SOCKET sock, SOCKADDR_IN addrInfo);
	
	private:

		static DWORD WINAPI acceptThread(LPVOID server);
		static DWORD WINAPI workerThread(LPVOID server);

	private:
		SOCKET m_listenSocket;
		HANDLE m_hIocp;
		
	};
}