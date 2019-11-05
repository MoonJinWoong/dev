
#include "NetService.h"

bool NetService::InitSocket()
{
	WSADATA wsaData;

	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		printf("[에러] WSAStartup()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	//연결지향형 TCP , Overlapped I/O 소켓을 생성
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mListenSocket)
	{
		printf("[에러] socket()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	printf("소켓 초기화 성공\n");
	return true;
}

bool NetService::BindandListen(int nBindPort)
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(nBindPort); //서버 포트를 설정한다.		
	//어떤 주소에서 들어오는 접속이라도 받아들이겠다.
	//보통 서버라면 이렇게 설정한다. 만약 한 아이피에서만 접속을 받고 싶다면
	//그 주소를 inet_addr함수를 이용해 넣으면 된다.
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//위에서 지정한 서버 주소 정보와 cIOCompletionPort 소켓을 연결한다.
	int nRet = bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		printf("[에러] bind()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	//접속 요청을 받아들이기 위해 cIOCompletionPort소켓을 등록하고 
	//접속대기큐를 5개로 설정 한다.
	nRet = listen(mListenSocket, 5);
	if (0 != nRet)
	{
		printf("[에러] listen()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	printf("서버 등록 성공..\n");
	return true;
}


bool NetService::StartServer(const unsigned int maxClientCount)
{
	CreateClient(maxClientCount);

	//CompletionPort객체 생성 요청을 한다.
	auto ret = mIocp.CreateNewIocp(MAX_WORKERTHREAD);
	if(!ret)
	{
		return false;
	}
	
	
	//mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKERTHREAD);
	//if (NULL == mIOCPHandle)
	//{
	//	printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
	//	return false;
	//}

	//접속된 클라이언트 주소 정보를 저장할 구조체
	ret = CreateWokerThread();
	if (!ret) {
		return false;
	}

	ret = CreateAccepterThread();
	if (ret) {
		return false;
	}

	printf("서버 시작\n");
	return true;
}

void NetService::DestroyThread()
{
	mIsWorkerRun = false;
	CloseHandle(mIOCPHandle);

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


void NetService::CreateClient(const unsigned int maxClientCount)
{
	for (UINT32 i = 0; i < maxClientCount; ++i)
	{
		mClientInfos.emplace_back();
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

	printf("WokerThread 시작..\n");
	return true;
}

bool NetService::CreateAccepterThread()
{
	mAccepterThread = std::thread([this]() { AccepterThread(); });

	printf("AccepterThread 시작..\n");
	return true;
}


RemoteClient* NetService::GetEmptyClientInfo()
{
	for (auto& client : mClientInfos)
	{
		if (INVALID_SOCKET == client.stRemoteSock)
		{
			return &client;
		}
	}

	return nullptr;
}


bool NetService::BindIOCompletionPort(RemoteClient* pClientInfo)
{
	//socket과 pClientInfo를 CompletionPort객체와 연결시킨다.

	auto ret = mIocp.AddDeviceRemoteSocket(pClientInfo);
	if (!ret)
	{
		return false;
	}

	//auto hIOCP = CreateIoCompletionPort((HANDLE)pClientInfo->stRemoteSock
	//	, mIOCPHandle
	//	, (ULONG_PTR)(pClientInfo), 0);

	//if (NULL == hIOCP || mIOCPHandle != hIOCP)
	//{
	//	printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
	//	return false;
	//}

	return true;
}


bool NetService::BindRecv(RemoteClient* pClientInfo)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	pClientInfo->stRecvOver.m_wsaBuf.len = MAX_SOCKBUF;
	pClientInfo->stRecvOver.m_wsaBuf.buf = pClientInfo->stRecvOver.m_RecvBuf;
	pClientInfo->stRecvOver.m_eOperation = IOOperation::RECV;

	int nRet = WSARecv(pClientInfo->stRemoteSock,
		&(pClientInfo->stRecvOver.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (pClientInfo->stRecvOver),
		NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}


bool NetService::SendMsg(RemoteClient* pClientInfo, char* pMsg, int nLen)
{
	DWORD dwRecvNumBytes = 0;

	//전송될 메세지를 복사
	CopyMemory(pClientInfo->stSendOver.m_SendBuf, pMsg, nLen);


	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	pClientInfo->stSendOver.m_wsaBuf.len = nLen;
	pClientInfo->stSendOver.m_wsaBuf.buf = pClientInfo->stSendOver.m_SendBuf;
	pClientInfo->stSendOver.m_eOperation = IOOperation::SEND;

	pClientInfo->stSendOver.m_SendBuf[nLen] = NULL;

	int nRet = WSASend(pClientInfo->stRemoteSock,
		&(pClientInfo->stSendOver.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED) & (pClientInfo->stSendOver),
		NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSASend()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}


	return true;
}


void NetService::WokerThread()
{
	//CompletionKey를 받을 포인터 변수
	RemoteClient* pClientInfo = NULL;
	//함수 호출 성공 여부
	bool bSuccess = TRUE;
	//Overlapped I/O작업에서 전송된 데이터 크기
	unsigned long dwIoSize = 0;
	//I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
	LPOVERLAPPED lpOverlapped = NULL;

	while (mIsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(mIocp.m_workIocp,
			&dwIoSize,					// 실제로 전송된 바이트
			(PULONG_PTR)&pClientInfo,		// CompletionKey
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
		if (FALSE == bSuccess || (0 == dwIoSize && TRUE == bSuccess))
		{
			printf("socket(%d) 접속 끊김\n", (int)pClientInfo->stRemoteSock);
			CloseSocket(pClientInfo);
			continue;
		}


		CustomOverEx* pOverlappedEx = (CustomOverEx*)lpOverlapped;

		//Overlapped I/O Recv작업 결과 뒤 처리
		if (IOOperation::RECV == pOverlappedEx->m_eOperation)
		{
			pOverlappedEx->m_RecvBuf[dwIoSize] = NULL;
			printf("[수신] bytes : %d , msg : %s\n", dwIoSize, pOverlappedEx->m_RecvBuf);
			//클라이언트에 메세지를 에코한다.
			SendMsg(pClientInfo, pOverlappedEx->m_RecvBuf, dwIoSize);
			BindRecv(pClientInfo);
		}
		//Overlapped I/O Send작업 결과 뒤 처리
		else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
		{
			printf("[송신] bytes : %d , msg : %s\n", dwIoSize, pOverlappedEx->m_SendBuf);

		}
		//예외 상황
		else
		{
			printf("socket(%d)에서 예외상황\n", (int)pClientInfo->stRemoteSock);
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
		RemoteClient* pClientInfo = GetEmptyClientInfo();
		if (NULL == pClientInfo)
		{
			printf("[에러] Client Full\n");
			return;
		}

		//클라이언트 접속 요청이 들어올 때까지 기다린다.
		pClientInfo->stRemoteSock = accept(mListenSocket, (SOCKADDR*)&stClientAddr, &nAddrLen);
		if (INVALID_SOCKET == pClientInfo->stRemoteSock)
		{
			continue;
		}

		//I/O Completion Port객체와 소켓을 연결시킨다.
		bool bRet = BindIOCompletionPort(pClientInfo);
		if (false == bRet)
		{
			return;
		}

		//Recv Overlapped I/O작업을 요청해 놓는다.
		bRet = BindRecv(pClientInfo);
		if (false == bRet)
		{
			return;
		}

		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(stClientAddr.sin_addr), clientIP, 32 - 1);
		printf("클라이언트 접속 : IP(%s) SOCKET(%d)\n", clientIP, (int)pClientInfo->stRemoteSock);

		//클라이언트 갯수 증가
		++mClientCnt;
	}
}


void NetService::CloseSocket(RemoteClient* pClientInfo, bool bIsForce)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown(pClientInfo->stRemoteSock, SD_BOTH);

	//소켓 옵션을 설정한다.
	setsockopt(pClientInfo->stRemoteSock, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//소켓 연결을 종료 시킨다. 
	closesocket(pClientInfo->stRemoteSock);

	pClientInfo->stRemoteSock = INVALID_SOCKET;
}