

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

		// socket 생성
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
		

		// iocp 객체 생성
		m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
		if (m_hIocp == nullptr)
		{
		   std::cout << "1st CreateIoCompletionPort error !" << std::endl;
		   return false;
		}

		// session 관리해줄 매니저 
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
			std::cout << i << "번 째 Wokrker thread" << std::endl;
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
			
			
			// 들어온 client들 처리 
			ProcAccept(acceptedSock, recvAddr);
		}

		return 0;
	}

	void IOCP::ProcAccept(SOCKET accepted_sock, SOCKADDR_IN addrInfo)
	{

		//  세션을 매니저에 있는 세션 풀에  등록
		auto ret2 = m_SessionMgr->addSession(++m_ConectedSeq);


		// 가져와서 iocp 와 결합
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


		//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
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
		//CompletionKey를 받을 포인터 변수
		Session* pSession = nullptr;
		//함수 호출 성공 여부
		bool bSuccess = true;
		//Overlapped I/O작업에서 전송된 데이터 크기
		DWORD dwIoSize = 0;
		//I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
		LPOVERLAPPED lpOverlapped = nullptr;


		while (m_bWorkerRun)
		{
			//////////////////////////////////////////////////////
			//이 함수로 인해 쓰레드들은 WaitingThread Queue에
			//대기 상태로 들어가게 된다.
			//완료된 Overlapped I/O작업이 발생하면 IOCP Queue에서
			//완료된 작업을 가져와 뒤 처리를 한다pSession
			//그리고 PostQueuedCompletionStatus()함수에의해 사용자
			//메세지가 도착되면 쓰레드를 종료한다.
			//////////////////////////////////////////////////////
			bSuccess = GetQueuedCompletionStatus(m_hIocp,
				&dwIoSize,					// 실제로 전송된 바이트
				(PULONG_PTR)& pSession,		// CompletionKey
				&lpOverlapped,				// Overlapped IO 객체
				INFINITE);					// 대기할 시간

			//client가 접속을 끊었을때..			
			if (false == bSuccess && 0 == dwIoSize)
			{
				std::cout << "Client[" << pSession->getUniqueId() << "] ShutDown" << std::endl;
				ShutdownSocket(pSession , true);
				continue;
			}

			//사용자 쓰레드 종료 메세지 처리..
			if (true == bSuccess && 0 == dwIoSize &&
				nullptr == lpOverlapped)
			{
				m_bWorkerRun = false;
				continue;
			}
			if (NULL == lpOverlapped)
				continue;


			// 여기서부터 





			//stOverlappedEx* pOverlappedEx =
			//	(stOverlappedEx*)lpOverlapped;

			////Overlapped I/O Recv작업 결과 뒤 처리
			//if (OP_RECV == pOverlappedEx->m_eOperation)
			//{
			//	pOverlappedEx->m_szBuf[dwIoSize] = NULL;
			//	m_pMainDlg->OutputMsg("[수신] bytes : %d , msg : %s"
			//		, dwIoSize, pOverlappedEx->m_szBuf);

			//	//클라이언트에 메세지를 에코한다.
			//	BindRecv(pClientInfo);
			//}
			////Overlapped I/O Send작업 결과 뒤 처리
			//else if (OP_SEND == pOverlappedEx->m_eOperation)
			//{
			//	m_pMainDlg->OutputMsg("[송신] bytes : %d , msg : %s"
			//		, dwIoSize, pOverlappedEx->m_szBuf);
			//}
			////예외 상황
			//else
			//{
			//	m_pMainDlg->OutputMsg("socket(%d)에서 예외상황",
			//		pClientInfo->m_socketClient);
			//}
		}

		return 1;
	}

	void IOCP::ShutdownSocket(Session* session, bool isForce)
	{
		struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

		// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여
		// 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
		if (true == isForce)
			stLinger.l_onoff = 1;

		//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
		shutdown(session->m_socket, SD_BOTH);
		//소켓 옵션을 설정한다.
		setsockopt(session->m_socket, SOL_SOCKET,
			SO_LINGER, (char*)& stLinger, sizeof(stLinger));
		//소켓 연결을 종료 시킨다. 
		closesocket(session->m_socket);


		std::cout << "client seq[" << session->getUniqueId() << "] shutdown...!" << std::endl;
		session->m_socket = INVALID_SOCKET;
		session->setUniqueId(-1);
	}

}