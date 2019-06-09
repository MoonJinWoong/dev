

#include "Session.h"
#include "SessionMgr.h"
#include "IOCP.h"


namespace NetworkLayer
{
	IOCP::IOCP() {}
	IOCP::~IOCP() 
	{
		::closesocket(m_listenSocket);
	}

	bool IOCP::InitIocpServer()
	{
		WSADATA   wsadata;
		if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
		{
			std::cout << "WSAStartup error...!" << std::endl;
			return false;
		}

		// socket ����
		m_listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_listenSocket == INVALID_SOCKET)
		{
			std::cout << "WSASocket error !" << std::endl;
			std::cout << "err code : " << WSAGetLastError() << std::endl;
			return false;
		}

		// bind and listen
		if (BindAndListen() == false)
		{
			std::cout << "Bind And Listen() is failed..." << std::endl;
			return false;
		}
		

		// iocp ��ü ����
		m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
		if (m_hIocp == nullptr)
		{
		   std::cout << "1st CreateIoCompletionPort error !" << std::endl;
		   return false;
		}

		// session �������� �Ŵ��� 
		m_SessionMgr = std::make_unique<SessionMgr>();
		auto ret = m_SessionMgr->Init();
		if (ret == false)
		{
			std::cout << "SessionMgr init fail" << std::endl;
			return false;
		}
		return true;
	}

	bool IOCP::BindAndListen()
	{
		SOCKADDR_IN ServerAddr;
		ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(SERVERPORT);
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

		return true;
	}

	bool IOCP::CreateThreads()
	{
		m_bWorkerRun = true;


		//   worker thread
		std::vector<std::thread*> Worker_Threads;
		Worker_Threads.reserve(CPU_CORE);
		for (int i = 0; i < CPU_CORE; ++i)
		{ 
			Worker_Threads.push_back
			(
				new std::thread{ &IOCP::workerThread,this }
			); 
			std::cout << i << "�� ° Wokrker thread" << std::endl;
		}


		//  accept thread
		std::thread accept_Thread{ &IOCP::acceptThread,this };

		//  join 
		for (auto wor : Worker_Threads) { wor->join();  delete wor; }
		Worker_Threads.clear();
		accept_Thread.join();

		closeServer();

		std::cout << "thread shut down.." << std::endl;
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




	DWORD WINAPI IOCP::acceptThread()
	{
		//IOCP* server = (IOCP*)pServer;

		while (true)
		{
			SOCKET acceptedSock = INVALID_SOCKET;
			SOCKADDR_IN recvAddr;
			static int len = sizeof(recvAddr);

			acceptedSock = WSAAccept(m_listenSocket, (struct sockaddr*) & recvAddr
				, &len, NULL,0);

			if (acceptedSock == SOCKET_ERROR)
			{
				std::cout << " Accept is failed..." << std::endl;
				break;
			}

			std::cout << "asdfasdf" << std::endl;
			
			
			// ���� client�� ó�� 
			ProcAccept(acceptedSock, recvAddr);
		}

		return 0;
	}

	void IOCP::ProcAccept(SOCKET accepted_sock, SOCKADDR_IN addrInfo)
	{

		//  ������ �Ŵ����� �ִ� ���� Ǯ��  ���
		auto ret2 = m_SessionMgr->addSession(++m_ConectedSeq);


		// �����ͼ� iocp �� ����
		auto session = m_SessionMgr->GetSession(m_ConectedSeq);


		HANDLE h_obj = CreateIoCompletionPort((HANDLE)accepted_sock, m_hIocp,
			(ULONG_PTR) & (session), NULL);

		if (!h_obj)
		{
			delete session;
			return;
		}

		DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;


		//Overlapped I/O�� ���� �� ������ ������ �ش�.
		session->m_stRecvIO.s_wsaBuf.len = MAX_IO_SIZE;
		
		session->m_stRecvIO.s_wsaBuf.buf =
		session->m_stRecvIO.s_szBuf;
		session->m_stRecvIO.s_eOperation = READ;

		int nRet = WSARecv(accepted_sock,
			&(session->m_stRecvIO.s_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED) & (session->m_stRecvIO),
			NULL);

		if (nRet == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
				std::cout << "WSARecv Error in AcceptThread...!" << std::endl;
		}
		
		std::cout << "client seq[" << session->getUniqueId() << "]" << std::endl;
	}


	DWORD WINAPI IOCP::workerThread()
	{
		//CompletionKey�� ���� ������ ����
		Session* pSession = nullptr;
		//�Լ� ȣ�� ���� ����
		bool bSuccess = true;
		//Overlapped I/O�۾����� ���۵� ������ ũ��
		DWORD dwIoSize = 0;
		//I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
		LPOVERLAPPED lpOverlapped = nullptr;


		while (m_bWorkerRun)
		{
			//////////////////////////////////////////////////////
			//�� �Լ��� ���� ��������� WaitingThread Queue��
			//��� ���·� ���� �ȴ�.
			//�Ϸ�� Overlapped I/O�۾��� �߻��ϸ� IOCP Queue����
			//�Ϸ�� �۾��� ������ �� ó���� �Ѵ�pSession
			//�׸��� PostQueuedCompletionStatus()�Լ������� �����
			//�޼����� �����Ǹ� �����带 �����Ѵ�.
			//////////////////////////////////////////////////////
			bSuccess = GetQueuedCompletionStatus(m_hIocp,
				&dwIoSize,					// ������ ���۵� ����Ʈ
				(PULONG_PTR)& pSession,		// CompletionKey
				&lpOverlapped,				// Overlapped IO ��ü
				INFINITE);					// ����� �ð�

			//client�� ������ ��������..			
			if (false == bSuccess && 0 == dwIoSize)
			{
				std::cout << "Client[" << pSession->getUniqueId() << "] ShutDown" << std::endl;
				ShutdownSocket(pSession , true);
				continue;
			}

			//����� ������ ���� �޼��� ó��..
			if (true == bSuccess && 0 == dwIoSize &&
				nullptr == lpOverlapped)
			{
				m_bWorkerRun = false;
				continue;
			}
			if (NULL == lpOverlapped)
				continue;


			// ���⼭���� 





			//stOverlappedEx* pOverlappedEx =
			//	(stOverlappedEx*)lpOverlapped;

			////Overlapped I/O Recv�۾� ��� �� ó��
			//if (OP_RECV == pOverlappedEx->m_eOperation)
			//{
			//	pOverlappedEx->m_szBuf[dwIoSize] = NULL;
			//	m_pMainDlg->OutputMsg("[����] bytes : %d , msg : %s"
			//		, dwIoSize, pOverlappedEx->m_szBuf);

			//	//Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
			//	BindRecv(pClientInfo);
			//}
			////Overlapped I/O Send�۾� ��� �� ó��
			//else if (OP_SEND == pOverlappedEx->m_eOperation)
			//{
			//	m_pMainDlg->OutputMsg("[�۽�] bytes : %d , msg : %s"
			//		, dwIoSize, pOverlappedEx->m_szBuf);
			//}
			////���� ��Ȳ
			//else
			//{
			//	m_pMainDlg->OutputMsg("socket(%d)���� ���ܻ�Ȳ",
			//		pClientInfo->m_socketClient);
			//}
		}

		return 1;
	}

	void IOCP::ShutdownSocket(Session* session, bool isForce)
	{
		struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

		// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ�
		// ���� ���� ��Ų��. ���� : ������ �ս��� ������ ���� 
		if (true == isForce)
			stLinger.l_onoff = 1;

		//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
		shutdown(session->m_socket, SD_BOTH);
		//���� �ɼ��� �����Ѵ�.
		setsockopt(session->m_socket, SOL_SOCKET,
			SO_LINGER, (char*)& stLinger, sizeof(stLinger));
		//���� ������ ���� ��Ų��. 
		closesocket(session->m_socket);


		std::cout << "client seq[" << session->getUniqueId() << "] shutdown...!" << std::endl;
		session->m_socket = INVALID_SOCKET;
		session->setUniqueId(-1);
	}

}