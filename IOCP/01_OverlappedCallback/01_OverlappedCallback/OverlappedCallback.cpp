#include "stdafx.h"
#include <iostream>
#include "OverlappedCallback.h"



// 접속을 받기 위한 대기 스레드

unsigned int WINAPI CallAccepterThread(LPVOID p)
{
	auto pOverlappedCallback = (OverlappedCallback*)p;
	pOverlappedCallback->AccepterThread();
	return 0;
}

//Overlapped IO 작업 완료 통보를 받는 콜백 함수
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
	// 윈속의 사용을 종료
	WSACleanup();

	//listen 소켓을 닫음
	closesocket(m_sockListen);
	m_bAccepterRun = false;
	//thread 종료를 기다림
	WaitForSingleObject(m_hAccepterThread, INFINITE);
}

// 소켓을 초기화하는 함수
bool OverlappedCallback::InitSocket()
{
	WSADATA wsaData;
	
	//윈속을 2.2로 초기화 
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		printf("error! WSAStartUP() 실패 :$d", WSAGetLastError());
		return false;
	}

	// 연결지향형 TCP , Overlapped IO 소켓을 생성
	m_sockListen = WSASocket(AF_INET, SOCK_STREAM
		, IPPROTO_TCP, NULL, NULL , WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == m_sockListen)
	{
		printf("error socket() 실패 : $d", WSAGetLastError());
		return false;
	}

	// 윈도우 핸들 저장
	printf("소켓 초기화 성공 \n");
	return true;
}

