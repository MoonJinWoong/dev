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
















