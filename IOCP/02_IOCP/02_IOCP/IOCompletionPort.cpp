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









