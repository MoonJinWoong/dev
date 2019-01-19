#include "stdafx.h"
#include <iostream>
#include "OverlappedCallback.h"



// ������ �ޱ� ���� ��� ������

unsigned int WINAPI CallAccepterThread(LPVOID p)
{
	auto pOverlappedCallback = (OverlappedCallback*)p;
	pOverlappedCallback->AccepterThread();
	return 0;
}

//Overlapped IO �۾� �Ϸ� �뺸�� �޴� �ݹ� �Լ�
void CALLBACK CompletionRoutine(DWORD dwError,
								DWORD dwTransferred,
								LPWSAOVERLAPPED Overlapped,
								DWORD dwFlags);

OverlappedCallback::OverlappedCallback(void)
{
	//m_pMainDlg = NULL;
	m_bAccepterRun = true;
	m_nClientCnt = 0;
	m_hAccepterThread = NULL;
	m_sockListen = INVALID_SOCKET;
	ZeroMemory(m_szSocketBuf, 1024);

}

OverlappedCallback::~OverlappedCallback(void)
{
	// ������ ����� ����
	WSACleanup();

	//listen ������ ����
	closesocket(m_sockListen);
	m_bAccepterRun = false;
	//thread ���Ḧ ��ٸ�
	WaitForSingleObject(m_hAccepterThread, INFINITE);
}

// ������ �ʱ�ȭ�ϴ� �Լ�
bool OverlappedCallback::InitSocket()
{
	WSADATA wsaData;
	
	//������ 2.2�� �ʱ�ȭ 
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		printf("[����] WSAStartup()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	// ���������� TCP , Overlapped IO ������ ����
	m_sockListen = WSASocket(AF_INET, SOCK_STREAM
		, IPPROTO_TCP, NULL, NULL , WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == m_sockListen)
	{
		printf("[����] socket()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	// ������ �ڵ� ����
	printf("���� �ʱ�ȭ ���� \n");
	return true;
}

void OverlappedCallback::CloseSocket(SOCKET socketClose, bool blsForce)
{
	struct linger stLinger = { 0,0 }; // S0_DONTLINGER�� ����

	//blsForce �� true�̸� S0_LINGER, timeout = 0���� �����Ͽ�
	// ���� ��Ų��. ����: ������ �ս� ���ɼ� ����.
	if (true == blsForce)
		stLinger.l_onoff = 1;

	//socketClose������ ������ �ۼ����� ��� ��ž 
	shutdown(socketClose, SD_BOTH);
	// ���� �ɼ� ����
	setsockopt(socketClose, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
	// ���� ���� ����
	closesocket(socketClose);

	socketClose = INVALID_SOCKET;
}

// ������ �ּ� ������ ���ϰ� ���� ��Ű�� ���� ��û�� �ޱ� ���� �� ������ ����ϴ� �Լ�
bool OverlappedCallback::BindandListen(int nBindPort)
{
	SOCKADDR_IN stServerAddr;


	stServerAddr.sin_family = AF_INET;
	// ���� ��Ʈ ����
	stServerAddr.sin_port = htons(nBindPort);
	// � �ּҿ��� ������ �����̶� �޾Ƶ��δ�.
	// ���� ������� �̷��� ����
	// ���� �ϳ��� ip������ �ް� ������ �� �ּҸ� inet_addr �Լ��� �̿��ؼ� ������ �ȴ�.
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// ������ ������ ���� �ּ� ������ m_socketListen ������ �����Ѵ�.
	int nRet = bind(m_sockListen, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		printf("error!! bind() fail!!!! : $d\n", WSAGetLastError());
		return false;
	}


	// ���� ��û�� �޾Ƶ��̱� ���� m_socketListen ������ ����ϰ� ���Ӵ��ť�� 5���� ����
	nRet = listen(m_sockListen, 5);
	if (0 != nRet)
	{
		printf("error!!! listen() fail !!!!!! $d \n", WSAGetLastError());
		return false;
	}


	printf("server register success !!!! \n");
	return true;

}

bool OverlappedCallback::CreateAccepterThread()
{
	unsigned uiThreadId = 0;
	m_hAccepterThread = (HANDLE)_beginthreadex(NULL, 0, &CallAccepterThread, this,
		CREATE_SUSPENDED, &uiThreadId);

	if (m_hAccepterThread = NULL)
	{
		printf("AccepterThread Create id Failed : %u\n", GetLastError());
		return false;
	}

	ResumeThread(m_hAccepterThread);
	printf("AccepterThread ����...\n");
	return true;
}

bool OverlappedCallback::StartServer()
{
	// ���ӵ� Ŭ�� �ּ� ������ ������ ����ü

	bool bRet = CreateAccepterThread();
	if (false == bRet)
		return false;

	printf("Server Start !!!! \n");
	return true;
}

bool OverlappedCallback::BindRecv(SOCKET socket)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	// Overlapped I/O�� ���� ����ü ����
	stOverlappedEx* pOverlappedEx = new stOverlappedEx;
	// ����ü �ʱ�ȭ 
	ZeroMemory(pOverlappedEx, sizeof(WSAOVERLAPPED));


	// Overlapped I/O�� ���� �� ���� ����
	pOverlappedEx->m_wsaBuf.len = MAX_SOCKBUF;
	pOverlappedEx->m_wsaBuf.buf = &pOverlappedEx->m_szBuf[0];
	pOverlappedEx->m_socketClient = socket;
	pOverlappedEx->m_eOperation = OP_RECV;
	pOverlappedEx->m_pOverlappedCallback = this;

	int nRet = WSARecv(socket,
		&(pOverlappedEx->m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED)(pOverlappedEx),
		CompletionRoutine);

	// socket err �̸� client socket�� �������ɷ� ó���Ѵ�. 
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("error WSARect() �Լ� Failed !!!! $d \n", WSAGetLastError());
		return false;
	}
	return true;
}

bool OverlappedCallback::SendMsg(SOCKET socket, char* pMsg, int nLen)
{
	DWORD dwRevNumBytes = 0;

	//Overlapped I/O�� ���� ����ü ����
	stOverlappedEx* pOverlappedEx = new stOverlappedEx;
	// ����ü �ʱ�ȭ
	ZeroMemory(pOverlappedEx, sizeof(WSAOVERLAPPED));
	// ���۵� �޼����� ����
	CopyMemory(pOverlappedEx->m_szBuf, pMsg, nLen);

	// Overlapped I/O�� ���� �� ������ ���� ���ش�
	pOverlappedEx->m_wsaBuf.buf = pOverlappedEx->m_szBuf;
	pOverlappedEx->m_wsaBuf.len = nLen;
	pOverlappedEx->m_socketClient = socket;
	pOverlappedEx->m_eOperation = OP_SEND;
	pOverlappedEx->m_pOverlappedCallback = this;

	int nRet = WSASend(socket,
		&(pOverlappedEx->m_wsaBuf),
		1,
		&dwRevNumBytes,
		0,
		(LPWSAOVERLAPPED)(pOverlappedEx),
		CompletionRoutine);

	// socket err�̸� client socket�� �������ɷ� ó�� 
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf(" error WSASend() failed !!! %d\n ", WSAGetLastError());
		return false;
	}
	return true;
}

// ������� ������ �޴� thread 
void OverlappedCallback::AccepterThread()
{
	SOCKADDR_IN stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);
	while (m_bAccepterRun)
	{
		// Ŭ�� ���� ��û�� ���� ������ ��ٸ���
		SOCKET sockAccept = accept(m_sockListen,
			(SOCKADDR*)&stClientAddr, &nAddrLen);

		if (INVALID_SOCKET == sockAccept)
		{
			continue;
		}

		bool bRet = BindRecv(sockAccept);
		if (false == bRet)
		{
			return;
		}

		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(stClientAddr.sin_addr), clientIP, 32 - 1);
		printf("client connect !!! : [ip - %s]  [socket - $d]\n", clientIP, (int)sockAccept);

		SleepEx(0, true);
	}

}


void CALLBACK CompletionRoutine(DWORD dwError,
								DWORD dwTransfered,
								LPWSAOVERLAPPED lpOverlapped,
								DWORD dwFlags)
{
	stOverlappedEx* pOverlappedEx = (stOverlappedEx*)lpOverlapped;

	// class�� �����͸� �޴´�
	auto pOverlappedCallback =
		(OverlappedCallback*)pOverlappedEx->m_pOverlappedCallback;

	//COverlappedCallbackDlg* pMainDlg = pOverlappedCallback->GetMainDlg();

	// ������ ����
	if (0 == dwTransfered)
	{
		printf("connect fail! socket: $d\n ", (int)pOverlappedEx->m_socketClient);
		pOverlappedCallback->CloseSocket(pOverlappedEx->m_socketClient);
		goto end;
	}
	
	if (0 != dwError)
	{
		printf("error!! completionroutine failed !!!! $d\n", WSAGetLastError());
		goto end;
	}
	switch (pOverlappedEx->m_eOperation)
	{
		//WSARecv�� Overlapepd I/O�� �Ϸ� �Ǿ���. 
	case OP_RECV:
		{
			pOverlappedEx->m_szBuf[dwTransfered] = NULL;
			printf("(����) bytes : %d , msg : %s \n", dwTransfered, pOverlappedEx->m_szBuf);

			// Ŭ�󿡰� �޼����� ���� �Ѵ�. 
			pOverlappedCallback->SendMsg(pOverlappedEx->m_socketClient,
				pOverlappedEx->m_szBuf, dwTransfered);
		}
		break;
		//WSASend�� Overlapped I/O�� �Ϸ��
	case OP_SEND:
		{
		pOverlappedEx->m_szBuf[dwTransfered] = NULL;
		printf("(�۽�) bytes : $d , msg : %s \n", dwTransfered, pOverlappedEx->m_szBuf);
		// �ٽ� Recv Overlapped I/O �� �ɾ��ش�.
		pOverlappedCallback->BindRecv(pOverlappedEx->m_socketClient);
		}
		break;
	default:
		{
		printf("���� ���� ���� Operation !!!!! \n");
		}
		break;
	}



end:
	delete pOverlappedEx;
}

void OverlappedCallback::DestroyThread()
{
	closesocket(m_sockListen);
	m_bAccepterRun = false;
	// ������ ���Ḧ ��ٸ�
	WaitForSingleObject(m_hAccepterThread, INFINITE);
}














