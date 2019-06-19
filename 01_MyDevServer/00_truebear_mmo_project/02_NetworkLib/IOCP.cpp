#include "SingleTon.h"
#include "ServerFrame.h"

#include "Session.h"
#include "IOCP.h"


namespace Network
{
	IOCP::IOCP(Content* content) : ServerFrame(content)
	{

	}
	IOCP::~IOCP()
	{
		::closesocket(m_listenSock);
	}
	bool IOCP::CreateSocket()
	{
		m_listenSock = WSASocket(AF_INET, SOCK_STREAM, NULL, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_listenSock == INVALID_SOCKET)
		{
			std::cout << "WSASocket() error" << std::endl;
			return false;
		}

		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(9000);
		inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));  //pton ipV6도 지원해준다함

		auto reuseAddr = 1;
		setsockopt(m_listenSock, SOL_SOCKET, SO_REUSEADDR, (char*)& reuseAddr,
			(int)sizeof(reuseAddr));

		auto retVal = ::bind(m_listenSock, (SOCKADDR*)& addr, sizeof(addr));
		if (retVal == SOCKET_ERROR)
		{
			std::cout << "bind() error" << std::endl;
			return false;
		}
			

		retVal = ::listen(m_listenSock, SOMAXCONN);
		if (retVal == SOCKET_ERROR)
		{
			std::cout << "listen() error" << std::endl;
			return false;
		}

		return true;
	}

	bool IOCP::Start()
	{
		// iocp 객체 생성
		m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, CPU_CORE);
		if (m_hIocp == nullptr)
		{
			std::cout << "CreateIoCompletionPort is fail in Start()" << std::endl;
			return false;
		}

		if (!this->CreateSocket())
		{
			std::cout << "CreateSocket() fail" << std::endl;
			return false;
		}


		// thread create
		std::vector<std::thread*> workerThread;
		workerThread.reserve(CPU_CORE);
		for (int i = 0; i < 4; ++i) 
		{ 
			workerThread.push_back(new std::thread{ &IOCP::workerThread,this }); 
		}
		std::thread acceptThread{ &IOCP::acceptThread,this };


		std::cout << "Create thread Call...Complete...!!" << std::endl;

		//  join 
		for (auto wor : workerThread) { wor->join();  delete wor; }
		workerThread.clear();
		acceptThread.join();

		ShutDownServer();
		
		std::cout << " ShutDown thread ...Complete...!!" << std::endl;
		return true;
	}
	SOCKET IOCP::getSocket()
	{
		return m_listenSock;
	}
	HANDLE IOCP::getIocpHandle()
	{
		return m_hIocp;
	}

	void IOCP::acceptThread()
	{
		while (true)
		{
			SOCKET accpetSocket = INVALID_SOCKET;
			SOCKADDR_IN recvAddr;
			ZeroMemory(&recvAddr, sizeof(SOCKADDR_IN));
			recvAddr.sin_family = AF_INET;
			recvAddr.sin_port = htons(SERVERPORT);
			recvAddr.sin_addr.s_addr = INADDR_ANY;
			int addr_size = sizeof(recvAddr);


			SOCKET acceptSocket = WSAAccept(m_listenSock,
				reinterpret_cast<sockaddr*>(&recvAddr), &addr_size, NULL, NULL);
			if (accpetSocket == INVALID_SOCKET)
				std::cout << "WSAAccept error....!" << std::endl;

			// 세션 생성 , 세션 매니저에 등록을 위한 함수
			this->runAccept(acceptSocket, recvAddr);


		}
		return;
	}
	void IOCP::workerThread()
	{
		while (true)
		{
			ExOverIo* exOverIo = nullptr;
			Session* session = nullptr;
			DWORD transferSize;

			auto ret = GetQueuedCompletionStatus(m_hIocp, &transferSize,
				(PULONG_PTR)& session, (LPOVERLAPPED*)& exOverIo, INFINITE);

			if (!ret)
			{
				std::cout << "GQCS fail" << std::endl;
				continue;
			}

			if (transferSize == 0)
			{
				SessionMgr.getInstance().ShutDownSession(session);
				continue;
			}


			switch (exOverIo->s_eOperation)
			{
			case WRITE:

			case READ:
			{
				Package* package = session->ProcRecv((size_t)transferSize);
				if (package != nullptr)
				{
					this->PutPacket(package);
				}
			}
			continue;

			// error 처리 필요

			}

				


		}
	}

	void IOCP::ShutDownServer()
	{
		closesocket(m_listenSock);
		WSACleanup();
	}
}