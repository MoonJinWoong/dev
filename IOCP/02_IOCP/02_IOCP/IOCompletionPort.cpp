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

bool IOCompletionPort::BindIOCompletionPort(stClientInfo* pClientInfo)
{
	HANDLE hIOCP;
	
	//socket�� pClientInfo�� CompletionPort ��ü�� �����Ų��. 
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
	// CompletionPort ��ü ���� ��û�� �Ѵ�. 
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	
	if (m_hIOCP == NULL)
	{
		printf("error !!! CreateIoCompletionPort() Call failed !!! %d\n", GetLastError());
		return false;
	}
	
	// ���ӵ� client address ������ ������ ����ü 
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


	//Overlapped I/O�� ���� �� ������ �������ش�. 
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

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool IOCompletionPort::SendMsg(stClientInfo* pClientInfo, char* pMsg, int nLen)
{
	DWORD dwRecvNumBytes = 0;

	// ���۵� �޼����� ����
	CopyMemory(pClientInfo->m_stSendOverlappedEx.m_szBuf, pMsg, nLen);

	// Overlapped I/O������ ������ ���� ���� 
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


	// socket error �̸� client socket�� �������ɷ� ó��.
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


// ������� ������ �޴� thread
void IOCompletionPort::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);
	while (m_bAccepterRun)
	{
		// ������ ���� ����ü�� �ε����� ���´�. 
		stClientInfo* pClientInfo = GetEmptyClientInfo();
		if (NULL == pClientInfo)
		{
			printf("Client Full !!!! \m");
			return;
		}

		// Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���. 
		pClientInfo->m_socketClient = accept(m_socketListen,
			(SOCKADDR*)&stClientAddr, &nAddrLen);

		if (INVALID_SOCKET == pClientInfo->m_socketClient)
			continue;

		// IO CompletionPort ��ü�� ������ �����Ų��. 
		bool bRet = BindIOCompletionPort(pClientInfo);
		if (bRet == false)
			return;

		// Recv Overlapped IO �۾��� ��û�س��� 
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

	// CompletionKey�� ���� ������ ���� 
	stClientInfo* pClientInfo = NULL;
	// �Լ� ȣ�� ���� ���� 
	BOOL bSuccess = TRUE;
	// Overlapped IO �۾����� ���۵� ������ ũ��
	DWORD dwIoSize = 0;
	// IO �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
	LPOVERLAPPED lpOverlapped = NULL;

	while (m_bWorkerRun)
	{
		// �� �Լ��� ���� thread���� WaitingThread Queue�� ���� �ȴ�. 
		// ��� �Ϸ� ���·� ���� �ȴ�. 
		// �Ϸ�� Overlapped IO �۾��� �߻��ϸ� IOCP Queue���� 
		// �Ϸ�� �۾��� ������ �� ó���� �Ѵ�. 
		// �׸��� PostQeueuCompletionStatus()�� ���� ����� �޼����� �����ϸ� 
		// thread�� �����Ѵ�. 

		bSuccess = GetQueuedCompletionStatus(m_hIOCP,
			&dwIoSize,						// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pClientInfo,		// Completion Key
			&lpOverlapped,					// Overlapped IO ��ü
			INFINITE);						// ����� �ð�

		// ����� thread ���� �޼��� ó��...
		if (TRUE == bSuccess && dwIoSize == 0 &&
			NULL == lpOverlapped)
		{
			m_bWorkerRun = false;
			continue;
		}
		if (NULL == lpOverlapped)
			continue;

		//client�� ������ ������ ��
		if (FALSE == bSuccess || (0 == dwIoSize && TRUE == bSuccess))
		{
			printf("socket(%d) ���� ����....\n", (int)pClientInfo->m_socketClient);

			CloseSocket(pClientInfo);
			continue;
		}

		stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

		// Overlapped IO Recv �۾� ��� ��ó��
		if (OP_RECV == pOverlappedEx->m_eOperation)
		{
			pOverlappedEx->m_szBuf[dwIoSize] = NULL;
			printf("���� Byte : %d,  msg:  $s\n", dwIoSize, pOverlappedEx->m_szBuf);

			// client�� msg�� ���� �Ѵ�. 
			SendMsg(pClientInfo, pOverlappedEx->m_szBuf, dwIoSize);
			BindRecv(pClientInfo);
		}
		//Overlapped IO Send �۾� ��� ��ó�� 
		else if (OP_SEND == pOverlappedEx->m_eOperation)
		{
			pOverlappedEx->m_szBuf[dwIoSize] = NULL;
			printf("���� Byte : %d , msg : %s", dwIoSize, pOverlappedEx->m_szBuf);
		}

		else
		{
			printf("socket(%d)���� ���ܻ�Ȳ\n", (int)pClientInfo->m_socketClient);
		}


	}
}

void IOCompletionPort::DestroyThread()
{
	for (int i = 0; i < MAX_WORKERTHREAD; i++)
	{
		// WaitingThread Qeueu���� ������� thread�� 
		// ����� ���� msg ������. 
		PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);
	}

	for (int i; i < MAX_WORKERTHREAD; i++)
	{
		// thread �ڵ��� �ݰ� thread�� ����ɶ����� ��ٸ���. 
		CloseHandle(m_hWorkerThread[i]);
		WaitForSingleObject(m_hWorkerThread[i], INFINITE);
	}

	m_bAccepterRun = false;
	// Accepter thread�� ���� 
	closesocket(m_socketListen);
	// thread ���Ḧ ��ٸ���.
	WaitForSingleObject(m_hAccepterThread, INFINITE);
}









