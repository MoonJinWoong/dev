#include "stdafx.h"
#include <process.h>
#include "IOCompletionPort.h"


// WSARecv�� WSASend�� Overlapped I/O �۾� ó���� ���� thread
unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	auto pOverlappedEvent = (IOCompletionPort*)p;
	pOverlappedEvent->WorkerThread();
	return 0;

}

// Client�� ������ �ޱ� ���� thread
unsigned int WINAPI CallAccepterThread(LPVOID p)
{
	auto* pOverlappedEvent = (IOCompletionPort*)p;
	pOverlappedEvent->AccepterThread();
	return 0;
}

IOCompletionPort::IOCompletionPort(void)
{
	// ������ ����
	// ��� ��� ���� �ʱ�ȭ
	m_bWorkerRun = true;
	m_bAccepterRun = true;
	m_nClientCnt = 0;
	m_hAccepterThread = NULL;
	m_hIOCP = NULL;
	m_socketListen = INVALID_SOCKET;
	ZeroMemory(m_szSocketBuf, 1024);
	for (int i = 0; i < MAX_WORKERTHREAD; i++)
		m_hWorkerThread[i] = NULL;

	//client ������ ������ ����ü�� ����
	m_pClientInfo = new stClientInfo[MAX_CLIENT];
}

IOCompletionPort::~IOCompletionPort(void)
{
	// winsock�� ��� ���� 
	WSACleanup();

	// �� ����� ��ü�� ���� 
	if (m_pClientInfo)
	{
		delete[] m_pClientInfo;
		m_pClientInfo = NULL;
	}
}


// socket�� �ʱ�ȭ�ϴ� �Լ� 
bool IOCompletionPort::InitSocket()
{
	WSADATA wsaData;
	// winsock�� ���� 2.2�� �ʱ�ȭ�Ѵ�. 
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (nRet != 0)
	{
		printf("error !! WSAStartUp() call failed !!!! $d\n", WSAGetLastError());
		return false;
	}

	// ���� ������ TCP, Overlapped I/O ������ ����
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


// �ش� socket ���� 
void IOCompletionPort::CloseSocket(stClientInfo* pClientInfo, bool blsForce)
{
	struct linger stLinger = { 0,0 };		// SO_DONTLINGER�� ���� 

	// blsForce�� true �̸� SO_LINGER, timeout = 0 ���� �����ؼ� 
	// ���� ��Ų��. ������ �ս� ���ɼ��� ���� 
	if (blsForce == true)
		stLinger.l_onoff = 1;

	//socketClose ������ ������ �ۼ����� ��� �ߴ� ��Ų��. 
	shutdown(pClientInfo->m_socketClient, SD_BOTH);
	
	// ���� �ɼ��� �����Ѵ�. 
	setsockopt(pClientInfo->m_socketClient, SOL_SOCKET,
		SO_LINGER, (char *)&stLinger, sizeof(stLinger));

	// ���� �ɼ� ����
	closesocket(pClientInfo->m_socketClient);

	pClientInfo->m_socketClient = INVALID_SOCKET;

}

// ������ �ּ������� ���ϰ� �����Ű�� ���� ��û�� �ޱ� ���� ������ ����ϴ� �Լ� 
bool IOCompletionPort::BindandListen(int nBindPort)
{

	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	
	// ���� ��Ʈ ����
	stServerAddr.sin_port = htons(nBindPort);
	
	// � �ּҿ��� ������ �����̶� �޾Ƶ��̰ڴ�. 
	// ���� ������� �̷��� �����Ѵ�. ���࿡ �ϳ��� �����ǿ����� ������ �ް� ������ 
	// �� �ּҸ� inet_addr �Լ��� �̿��� ������ �ȴ�. 
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// ������ ������ ���� �ּ� ������ cIOCmpletionPort ������ �����Ѵ�. 

	int nRet = bind(m_socketListen, (SOCKADDR*)&stServerAddr,
		sizeof(SOCKADDR_IN));
	
	if (nRet != 0)
	{
		printf("error!!! bind() failed !! %d\n", WSAGetLastError());
		return false;
	}

	// ���� ��û�� �޾Ƶ��̱� ���� clCompletionPort ������ ����ϰ�
	// ���� ���ť�� 5���� �����Ѵ�. 
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
	//WaitingThread Queue�� ��� ���·� ���� ��������� ����
	// ����Ǵ� ���� cpu core * 2 +1  MSDN�� ����. 

	for (int i = 0; i < MAX_WORKERTHREAD; i++)
	{
		m_hWorkerThread[i] = (HANDLE)_beginthreadex(NULL, 0, &CallWorkerThread, this,
			CREATE_SUSPENDED, &uiThreadId);
		if (m_hWorkerThread == NULL)
		{
			printf("WorkerThread Caretion Failed !!!! %u\n", GetLastError());
			return false;
		}
		ResumeThread(m_hWorkerThread[i]); // thread�� ����� �Լ�
	}

	printf("WorkerThread Start....!!!! \n");
	return true;
}

bool IOCompletionPort::CreateAccepterThread()
{
	unsigned int uiThreadId = 0;
	//Client�� ���� ��û�� ���� thread ����
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









