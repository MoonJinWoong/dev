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
		printf("error! WSAStartUP() ���� :$d", WSAGetLastError());
		return false;
	}

	// ���������� TCP , Overlapped IO ������ ����
	m_sockListen = WSASocket(AF_INET, SOCK_STREAM
		, IPPROTO_TCP, NULL, NULL , WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == m_sockListen)
	{
		printf("error socket() ���� : $d", WSAGetLastError());
		return false;
	}

	// ������ �ڵ� ����
	printf("���� �ʱ�ȭ ���� \n");
	return true;
}

