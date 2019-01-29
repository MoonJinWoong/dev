#include "stdafx.h"
#include <process.h>
#include "IOCompletionPort.h"


// WSARecv와 WSASend의 Overlapped I/O 작업 처리를 위한 thread
unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	auto pOverlappedEvent = (IOCompletionPort*)p;
	pOverlappedEvent->WorkerThread();
	return 0;

}

// Client의 접속을 받기 위한 thread
unsigned int WINAPI CallAccepterThread(LPVOID p)
{
	auto* pOverlappedEvent = (IOCompletionPort*)p;
	pOverlappedEvent->AccepterThread();
	return 0;
}

IOCompletionPort::IOCompletionPort(void)
{
	// 생성자 정의
	// 모든 멤버 변수 초기화
	m_bWorkerRun = true;
	m_bAccepterRun = true;
	m_nClientCnt = 0;
	m_hAccepterThread = NULL;
	m_hIOCP = NULL;
	m_socketListen = INVALID_SOCKET;
	ZeroMemory(m_szSocketBuf, 1024);
	for (int i = 0; i < MAX_WORKERTHREAD; i++)
		m_hWorkerThread[i] = NULL;

	//client 정보를 저장한 구조체를 생성
	m_pClientInfo = new stClientInfo[MAX_CLIENT];
}

IOCompletionPort::~IOCompletionPort(void)
{
	// winsock의 사용 종료 
	WSACleanup();

	// 다 사용한 객체를 삭제 
	if (m_pClientInfo)
	{
		delete[] m_pClientInfo;
		m_pClientInfo = NULL;
	}
}


// socket을 초기화하는 함수 
bool IOCompletionPort::InitSocket()
{
	WSADATA wsaData;
	// winsock을 버젼 2.2로 초기화한다. 
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0)
	{
		printf("error !! WSAStartUp() call failed !!!! $d\n", WSAGetLastError());
		return false;
	}

	// 연결 지향형 TCP, Overlapped I/O 소켓을 생성
	m_socketListen = WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == m_socketListen)
	{
		printf("error !!! socket() failed !!! %d \n", WSAGetLastError());
		return false;
	}

	printf("socket init success !!!!!! \n");
	return true;
}


// 해당 socket 종료 
void IOCompletionPort::CloseSocket(stClientInfo* pClientInfo, bool blsForce)
{
	struct linger stLinger = { 0,0 };		// SO_DONTLINGER로 설정 

	// blsForce가 true 이면 SO_LINGER, timeout = 0 으로 설정해서 
	// 강종 시킨다. 데이터 손실 가능성이 있음 
	if (blsForce == true)
		stLinger.l_onoff = 1;

	//socketClose 소켓의 데이터 송수신을 모두 중단 시킨다. 
	shutdown(pClientInfo->m_socketClient, SD_BOTH);
	
	// 소켓 옵션을 설정한다. 
	setsockopt(pClientInfo->m_socketClient, SOL_SOCKET,
		SO_LINGER, (char *)&stLinger, sizeof(stLinger));

	// 소켓 옵션 종료
	closesocket(pClientInfo->m_socketClient);

	pClientInfo->m_socketClient = INVALID_SOCKET;

}

// 서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 소켓을 등록하는 함수 
bool IOCompletionPort::BindandListen(int nBindPort)
{

	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	
	// 서버 포트 설정
	stServerAddr.sin_port = htons(nBindPort);
	
	// 어떤 주소에서 들어오는 접속이라도 받아들이겠다. 
	// 보통 서버라면 이렇게 설정한다. 만약에 하나의 아이피에서만 접속을 받고 싶으면 
	// 그 주소를 inet_addr 함수를 이용해 넣으면 된다. 
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 위에서 지정한 서버 주소 정보와 cIOCmpletionPort 소켓을 연결한다. 

	int nRet = bind(m_socketListen, (SOCKADDR*)&stServerAddr,
		sizeof(SOCKADDR_IN));
	
	if (nRet != 0)
	{
		printf("error!!! bind() failed !! %d\n", WSAGetLastError());
		return false;
	}

	// 접속 요청을 받아들이기 위해 clCompletionPort 소켓을 등록하고
	// 접속 대기큐를 5개로 세팅한다. 
	nRet = listen(m_socketListen, 5);
	if (nRet != 0)
	{
		printf("error !!! listen() failed !!! %d\n", WSAGetLastError());
		return false;
	}

	printf("Server bind and listen success!!!!! \n");
	return true;

}


bool IOCompletionPort::CreateWorkerThread()
{
	unsigned int uiThreadId = 0;
	//WaitingThread Queue에 대기 상태로 넣을 쓰레드들을 생성
	// 권장되는 개수 cpu core * 2 +1  MSDN에 있음. 

	for (int i = 0; i < MAX_WORKERTHREAD; i++)
	{
		m_hWorkerThread[i] = (HANDLE)_beginthreadex(NULL, 0, &CallWorkerThread, this,
			CREATE_SUSPENDED, &uiThreadId);
		if (m_hWorkerThread == NULL)
		{
			printf("WorkerThread Caretion Failed !!!! %u\n", GetLastError());
			return false;
		}
		ResumeThread(m_hWorkerThread[i]); // thread를 깨우는 함수
	}

	printf("WorkerThread Start....!!!! \n");
	return true;
}

bool IOCompletionPort::CreateAccepterThread()
{
	unsigned int uiThreadId = 0;
	//Client의 접속 요청을 받을 thread 생성
	m_hAccepterThread = (HANDLE)_beginthreadex(NULL, 0, &CallAccepterThread, this,
		CREATE_SUSPENDED, &uiThreadId);
	if (m_hAccepterThread == NULL)
	{
		printf("AccepterThread Creation is Failed !!!! %u\n", GetLastError());
		return false;
	}

	ResumeThread(m_hAccepterThread);
	printf("AccepterThread Start...!!!!! \n");
	return true;

}

bool IOCompletionPort::BindIOCompletionPort(stClientInfo* pClientInfo)
{
	HANDLE hIOCP;
	
	//socket과 pClientInfo를 CompletionPort 객체와 연결시킨다. 
	hIOCP = CreateIoCompletionPort((HANDLE)pClientInfo->m_socketClient, m_hIOCP,
		(ULONG_PTR)(pClientInfo), 0);

	if (NULL == hIOCP || m_hIOCP != hIOCP)
	{
		printf("error!! CreateIoCompletionPort() Call failed !!!! %d\n", GetLastError());
		return false;
	}
	return true;
}

bool IOCompletionPort::StartServer()
{
	// CompletionPort 객체 생성 요청을 한다. 
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	
	if (m_hIOCP == NULL)
	{
		printf("error !!! CreateIoCompletionPort() Call failed !!! %d\n", GetLastError());
		return false;
	}
	
	// 접속된 client address 정보를 저장할 구조체 
	bool bRet = CreateWorkerThread();
	if (bRet == false)
		return false;
	
	bRet = CreateAccepterThread();
	if (bRet == false)
		return false;


	printf("Server Start!!!!! \n");
	return true;
}


bool IOCompletionPort::BindRecv(stClientInfo* pClientInfo)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;


	//Overlapped I/O를 위해 각 정보를 세팅해준다. 
	pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
	pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.buf =
		pClientInfo->m_stRecvOverlappedEx.m_szBuf;
	pClientInfo->m_stRecvOverlappedEx.m_eOperation = OP_RECV;

	int nRet = WSARecv(pClientInfo->m_socketClient,
		&(pClientInfo->m_stRecvOverlappedEx.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPOVERLAPPED)&(pClientInfo->m_stRecvOverlappedEx), NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool IOCompletionPort::SendMsg(stClientInfo* pClientInfo, char* pMsg, int nLen)
{
	DWORD dwRecvNumBytes = 0;

	// 전송될 메세지를 복사
	CopyMemory(pClientInfo->m_stSendOverlappedEx.m_szBuf, pMsg, nLen);

	// Overlapped I/O를위한 각각의 정보 세팅 
	pClientInfo->m_stSendOverlappedEx.m_wsaBuf.len = nLen;
	pClientInfo->m_stSendOverlappedEx.m_wsaBuf.buf =
		pClientInfo->m_stSendOverlappedEx.m_szBuf;
	pClientInfo->m_stSendOverlappedEx.m_eOperation = OP_SEND;

	int nRet = WSASend(pClientInfo->m_socketClient,
		&(pClientInfo->m_stSendOverlappedEx.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED)&(pClientInfo->m_stSendOverlappedEx),
		NULL);


	// socket error 이면 client socket이 끊어진걸로 처리.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("error WSASend() call failed !!!! %d\n", WSAGetLastError());
		return false;
	}
	return true;
}


stClientInfo* IOCompletionPort::GetEmptyClientInfo()
{
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		if (INVALID_SOCKET == m_pClientInfo[i].m_socketClient)
			return &m_pClientInfo[i];
	}

	return NULL;
}


// 사용자의 접속을 받는 thread
void IOCompletionPort::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);
	while (m_bAccepterRun)
	{
		// 접속을 받을 구조체의 인덱스를 얻어온다. 
		stClientInfo* pClientInfo = GetEmptyClientInfo();
		if (NULL == pClientInfo)
		{
			printf("Client Full !!!! \m");
			return;
		}

		// 클라이언트 접속 요청이 들어올 때까지 기다린다. 
		pClientInfo->m_socketClient = accept(m_socketListen,
			(SOCKADDR*)&stClientAddr, &nAddrLen);

		if (INVALID_SOCKET == pClientInfo->m_socketClient)
			continue;

		// IO CompletionPort 객체와 소켓을 연결시킨다. 
		bool bRet = BindIOCompletionPort(pClientInfo);
		if (bRet == false)
			return;

		// Recv Overlapped IO 작업을 요청해놓음 
		bRet = BindRecv(pClientInfo);
		if (bRet == false)
			return;

		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(stClientAddr), clientIP, 32 - 1);
		printf("client connected : IP-%s  SOCKET-%d    \n", clientIP,
			(int)pClientInfo->m_socketClient);


		m_nClientCnt++;
	}
}

void IOCompletionPort::WorkerThread()
{

	// CompletionKey를 받을 포인터 변수 
	stClientInfo* pClientInfo = NULL;
	// 함수 호출 성공 여부 
	BOOL bSuccess = TRUE;
	// Overlapped IO 작업에서 전송된 데이터 크기
	DWORD dwIoSize = 0;
	// IO 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
	LPOVERLAPPED lpOverlapped = NULL;

	while (m_bWorkerRun)
	{
		// 이 함수로 인해 thread들은 WaitingThread Queue에 들어가게 된다. 
		// 대기 완료 상태로 들어가게 된다. 
		// 완료된 Overlapped IO 작업이 발생하면 IOCP Queue에서 
		// 완료된 작업을 가져와 뒤 처리를 한다. 
		// 그리고 PostQeueuCompletionStatus()에 의해 사용자 메세지가 도착하면 
		// thread를 종료한다. 

		bSuccess = GetQueuedCompletionStatus(m_hIOCP,
			&dwIoSize,						// 실제로 전송된 바이트
			(PULONG_PTR)&pClientInfo,		// Completion Key
			&lpOverlapped,					// Overlapped IO 객체
			INFINITE);						// 대기할 시간

		// 사용자 thread 종료 메세지 처리...
		if (TRUE == bSuccess && dwIoSize == 0 &&
			NULL == lpOverlapped)
		{
			m_bWorkerRun = false;
			continue;
		}
		if (NULL == lpOverlapped)
			continue;

		//client가 접속을 끊었을 때
		if (FALSE == bSuccess || (0 == dwIoSize && TRUE == bSuccess))
		{
			printf("socket(%d) 접속 끊김....\n", (int)pClientInfo->m_socketClient);

			CloseSocket(pClientInfo);
			continue;
		}

		stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

		// Overlapped IO Recv 작업 결과 뒤처리
		if (OP_RECV == pOverlappedEx->m_eOperation)
		{
			pOverlappedEx->m_szBuf[dwIoSize] = NULL;
			printf("수신 Byte : %d,  msg:  $s\n", dwIoSize, pOverlappedEx->m_szBuf);

			// client에 msg를 에코 한다. 
			SendMsg(pClientInfo, pOverlappedEx->m_szBuf, dwIoSize);
			BindRecv(pClientInfo);
		}
		//Overlapped IO Send 작업 결과 뒤처리 
		else if (OP_SEND == pOverlappedEx->m_eOperation)
		{
			pOverlappedEx->m_szBuf[dwIoSize] = NULL;
			printf("수신 Byte : %d , msg : %s", dwIoSize, pOverlappedEx->m_szBuf);
		}

		else
		{
			printf("socket(%d)에서 예외상황\n", (int)pClientInfo->m_socketClient);
		}


	}
}

void IOCompletionPort::DestroyThread()
{
	for (int i = 0; i < MAX_WORKERTHREAD; i++)
	{
		// WaitingThread Qeueu에서 대기중인 thread에 
		// 사용자 종료 msg 보낸다. 
		PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);
	}

	for (int i; i < MAX_WORKERTHREAD; i++)
	{
		// thread 핸들을 닫고 thread가 종료될때까지 기다린다. 
		CloseHandle(m_hWorkerThread[i]);
		WaitForSingleObject(m_hWorkerThread[i], INFINITE);
	}

	m_bAccepterRun = false;
	// Accepter thread를 종료 
	closesocket(m_socketListen);
	// thread 종료를 기다린다.
	WaitForSingleObject(m_hAccepterThread, INFINITE);
}









