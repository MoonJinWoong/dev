#pragma once

#include "preCompile.h"

namespace Core
{
	class ServerFrame;
}

namespace NetworkLayer
{
	class Session;
	class SessionMgr;

	class IOCP 
	{
	public:
		IOCP();
		virtual ~IOCP();
		bool InitIocpServer();
		bool BindAndListen();
		bool CreateThreads();
		bool startServer();
		void closeServer();
		void ShutdownSocket(Session* sessionInfo, bool isForce);
		void ProcAccept(SOCKET sock, SOCKADDR_IN addrInfo);
	
	private:
		 DWORD WINAPI acceptThread();
		 DWORD WINAPI workerThread();

	private:
		SOCKET m_listenSocket;
		HANDLE m_hIocp;

		//std::unique_ptr<Session> m_Session;
		std::unique_ptr<SessionMgr> m_SessionMgr;
		long long m_ConectedSeq{ 0 };
		//std::deque<int> m_SessionPoolIndex;       // 클라 식별 번호

		bool m_bWorkerRun{ false };


		
		
	};
}