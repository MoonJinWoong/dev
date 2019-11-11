
#include "NetService.h"

bool NetService::InitSocket()
{
	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		std::cout << "[err] WSAStartUP() fail..." << WSAGetLastError();
		return false;
	}

	//연결지향형 TCP , Overlapped I/O 소켓을 생성
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mListenSocket)
	{
		std::cout << "[err] WSASocket() fail..." << WSAGetLastError();
		return false;
	}

	std::cout << "[Success] InitSocket() "<<std::endl;
	return true;
}

bool NetService::BindandListen(c_u_Int nBindPort)
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(nBindPort); //서버 포트를 설정한다.		
	//어떤 주소에서 들어오는 접속이라도 받아들이겠다.
	//보통 서버라면 이렇게 설정한다. 만약 한 아이피에서만 접속을 받고 싶다면
	//그 주소를 inet_addr함수를 이용해 넣으면 된다.
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//위에서 지정한 서버 주소 정보와 cIOCompletionPort 소켓을 연결한다.
	int nRet = ::bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		std::cout << "[err] bind() fail..." << WSAGetLastError();
		return false;
	}

	//접속 요청을 받아들이기 위해 cIOCompletionPort소켓을 등록하고 
	//접속대기큐를 5개로 설정 한다.
	nRet = ::listen(mListenSocket, 5);
	if (0 != nRet)
	{
		std::cout << "[err] listen() fail..." << WSAGetLastError();
		return false;
	}

	std::cout << "[Success] BindandListen() " << std::endl;
	return true;
}

bool NetService::StartNetService(c_u_Int maxClientCount)
{
	CreateClient(maxClientCount);

	//CompletionPort객체 생성 요청을 한다.
	if (!mIocp.CreateNewIocp(MAX_WORKERTHREAD))
	{
		return false;
	}

	if(!CreateWokerThread())
	{
		return false;
	}

	if(!CreateAccepterThread())
	{
		return false;
	}

	std::cout << "[Success] StartNetService() " << std::endl;
	return true;
}

void NetService::DestroyThread()
{
	mIsWorkerRun = false;
	CloseHandle(mIocp.m_workIocp);

	for (auto& th : mIOWorkerThreads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	//Accepter 쓰레드를 종요한다.
	mIsAccepterRun = false;
	closesocket(mListenSocket);

	if (mAccepterThread.joinable())
	{
		mAccepterThread.join();
	}
}

void NetService::CreateClient(c_u_Int maxClientCount)
{
	for (int i = 0; i < maxClientCount; ++i)
	{
		auto pSession = new RemoteSession;
		pSession->SetUniqueId(i);
		mVecSessions.emplace_back(pSession);
	}
}

bool NetService::CreateWokerThread()
{
	unsigned int uiThreadId = 0;
	//WaingThread Queue에 대기 상태로 넣을 쓰레드들 생성 권장되는 개수 : (cpu개수 * 2) + 1 
	for (int i = 0; i < MAX_WORKERTHREAD; i++)
	{
		mIOWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	std::cout << "[Success] WokerThread() " << std::endl;
	return true;
}

bool NetService::CreateAccepterThread()
{
	mAccepterThread = std::thread([this]() { AccepterThread(); });

	std::cout << "[Success] AccepterThread() " << std::endl;
	return true;
}

RemoteSession* NetService::GetEmptyClientInfo()
{
	for (auto& client : mVecSessions)
	{
		if (INVALID_SOCKET == client->GetSock())
		{
			return client;
		}
	}
	return nullptr;
}


bool NetService::DoRecv(RemoteSession* pClientInfo)
{
	return pClientInfo->RecvMsg();
}

bool NetService::DoSend(RemoteSession* pClientInfo, char* pMsg, c_Int nLen)
{
	return pClientInfo->SendMsg(nLen, pMsg);
}

void NetService::WokerThread()
{
	//CompletionKey를 받을 포인터 변수
	RemoteSession* pSession = NULL;
	//함수 호출 성공 여부
	bool bSuccess = true;
	//Overlapped I/O작업에서 전송된 데이터 크기
	unsigned long dwIoSize = 0;
	//I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
	LPOVERLAPPED lpOverlapped = NULL;

	while (mIsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(mIocp.m_workIocp,
			&dwIoSize,					// 실제로 전송된 바이트
			(PULONG_PTR)&pSession,		// CompletionKey
			&lpOverlapped,				// Overlapped IO 객체
			INFINITE);					// 대기할 시간

		//사용자 쓰레드 종료 메세지 처리..
		if (bSuccess == false && 0 == dwIoSize && NULL == lpOverlapped)
		{
			mIsWorkerRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		//client가 접속을 끊었을때..			
		if (FALSE == bSuccess || (0 == dwIoSize && true == bSuccess))
		{
			OnClose(pSession->GetUniqueId());
			CloseSocket(pSession);
			continue;
		}


		CustomOverEx* pOverlappedEx = (CustomOverEx*)lpOverlapped;

		//Overlapped I/O Recv작업 결과 뒤 처리
		if (IOOperation::RECV == pOverlappedEx->m_eOperation)
		{
			pOverlappedEx->m_RecvBuf[dwIoSize] = NULL;			
			OnRecv(pSession->GetUniqueId(), dwIoSize, pOverlappedEx->m_RecvBuf);

			// echo
			DoSend(pSession, pOverlappedEx->m_RecvBuf, dwIoSize);
			
			//recv
			DoRecv(pSession);
		}
		//Overlapped I/O Send작업 결과 뒤 처리
		else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
		{
			std::cout << "[Send] Byte : " << dwIoSize << ", Msg :" << pOverlappedEx->m_SendBuf << std::endl;
		}
		//예외 상황
		else
		{
			std::cout << "[Err] SocketFd : " << (int)pSession->GetSock() << std::endl;
		}
	}
}

void NetService::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);

	while (mIsAccepterRun)
	{
		//접속을 받을 구조체의 인덱스를 얻어온다.
		RemoteSession* pClientInfo = GetEmptyClientInfo();
		if (NULL == pClientInfo)
		{
			std::cout << "[Err] Client Pull... UU " << std::endl;
			return;
		}

		//클라이언트 접속 요청이 들어올 때까지 기다린다.
		pClientInfo->GetSock() = accept(mListenSocket, (SOCKADDR*)&stClientAddr, &nAddrLen);
		if (INVALID_SOCKET == pClientInfo->GetSock())
		{
			continue;
		}

		//I/O Completion Port객체와 소켓을 연결시킨다.
		if (!mIocp.AddDeviceRemoteSocket(pClientInfo))
		{
			return;
		}

		//Recv Overlapped I/O작업을 요청해 놓는다.
		if (!DoRecv(pClientInfo))
		{
			return;
		}



		OnAccept(pClientInfo->GetUniqueId());

		//클라이언트 갯수 증가
		++mClientCnt;
	}
}

void NetService::CloseSocket(RemoteSession* pSession, bool bIsForce)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown(pSession->GetSock(), SD_BOTH);

	//소켓 옵션을 설정한다.
	setsockopt(pSession->GetSock(), SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//소켓 연결을 종료 시킨다. 
	closesocket(pSession->GetSock());

	pSession->GetSock() = INVALID_SOCKET;
}


bool NetService::SendMsg(c_u_Int uniqueId, c_u_Int size, char* pData)
{
	auto pSession = GetSessionByIdx(uniqueId);
	return pSession->SendMsg(size, pData);
}