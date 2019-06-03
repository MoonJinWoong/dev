
//#include "ServerOpt.h"
#include "preCompile.h"
#include "IOCP.h"


namespace NetLib
{
	IOCP::IOCP() {}
	IOCP::~IOCP() 
	{
		::closesocket(m_listenSocket);
	}

	bool IOCP::InitIocpServer()
	{
		m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, NULL, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_listenSocket == INVALID_SOCKET)
		{
			std::cout << "WSASocket error !" << std::endl;
			return false;
		}

		SOCKADDR_IN ServerAddr;
		ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(m_port);
		ServerAddr.sin_addr.s_addr = INADDR_ANY;


		auto bindRet = ::bind(m_listenSocket, reinterpret_cast<sockaddr*>(&ServerAddr), sizeof(ServerAddr));
		if (bindRet == SOCKET_ERROR)
		{
			std::cout << "bind error !" << std::endl;
			return false;
		}

		auto listenRet = listen(m_listenSocket, SOMAXCONN);
		if (listenRet == SOCKET_ERROR)
		{
			std::cout << "listen error !" << std::endl;
			return false;
		}

		m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
		if (m_hIocp == nullptr)
		{
		   std::cout << "1st CreateIoCompletionPort error !" << std::endl;
		   return false;
		}


		return true;
	}

	bool IOCP::createThreads()
	{
		//   worker thread
		std::vector<std::thread*> Worker_Threads;
		Worker_Threads.reserve(m_cpu_core);
		for (int i = 0; i < m_cpu_core ; ++i) 
		{ 
			Worker_Threads.push_back
			(
				new std::thread{ &IOCP::workerThread,this }
			); 
		}


		//  accept thread
		std::thread accept_Thread{ &IOCP::acceptThread,this };

		//  join 
		for (auto wor : Worker_Threads) { wor->join();  delete wor; }
		Worker_Threads.clear();
		accept_Thread.join();

		closeServer();

		std::cout << "Create thread Call...Complete...!!" << std::endl;
		return true;
	}





	bool IOCP::startServer()
	{

		return true;
	}
	void IOCP::closeServer()
	{
		closesocket(m_listenSocket);
		WSACleanup();
	}








	DWORD WINAPI IOCP::acceptThread(LPVOID server)
	{
		return 1;
	}
	DWORD WINAPI IOCP::workerThread(LPVOID server)
	{
		return 1;
	}


}