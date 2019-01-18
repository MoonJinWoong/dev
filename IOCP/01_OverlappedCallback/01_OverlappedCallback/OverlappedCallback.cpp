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
		printf("[에러] WSAStartup()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	// 연결지향형 TCP , Overlapped IO 소켓을 생성
	m_sockListen = WSASocket(AF_INET, SOCK_STREAM
		, IPPROTO_TCP, NULL, NULL , WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == m_sockListen)
	{
		printf("[에러] socket()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	// 윈도우 핸들 저장
	printf("소켓 초기화 성공 \n");
	return true;
}

void OverlappedCallback::CloseSocket(SOCKET socketClose, bool blsForce)
{
	struct linger stLinger = { 0,0 }; // S0_DONTLINGER로 설정

	//blsForce 가 true이면 S0_LINGER, timeout = 0으로 설정하여
	// 강종 시킨다. 주의: 데이터 손실 가능성 있음.
	if (true == blsForce)
		stLinger.l_onoff = 1;

	//socketClose소켓의 데이터 송수신을 모두 스탑 
	shutdown(socketClose, SD_BOTH);
	// 소켓 옵션 설정
	setsockopt(socketClose, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
	// 소켓 연결 종료
	closesocket(socketClose);

	socketClose = INVALID_SOCKET;
}

// 서버의 주소 정보를 소켓과 연결 시키고 접속 요청을 받기 위해 그 소켓을 등록하는 함수
bool OverlappedCallback::BindandListen(int nBindPort)
{
	SOCKADDR_IN stServerAddr;


	stServerAddr.sin_family = AF_INET;
	// 서버 포트 설정
	stServerAddr.sin_port = htons(nBindPort);
	// 어떤 주소에서 들어오는 접속이라도 받아들인다.
	// 보통 서버라면 이렇게 설정
	// 만약 하나의 ip에서만 받고 싶으면 그 주소를 inet_addr 함수를 이용해서 넣으면 된다.
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 위에서 지정한 서버 주소 정보와 m_socketListen 소켓을 연결한다.
	int nRet = bind(m_sockListen, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		printf("error!! bind() fail!!!! : $d\n", WSAGetLastError());
		return false;
	}


	// 접속 요청을 받아들이기 위해 m_socketListen 소켓을 등록하고 접속대기큐를 5개로 설정
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
	printf("AccepterThread 시작...\n");
	return true;
}

bool OverlappedCallback::StartServer()
{
	// 접속된 클라 주소 정보를 저장할 구조체

	bool bRet = CreateAccepterThread();
	if (false == bRet)
		return false;

	printf("Server Start !!!! \n");
	return true;
}
















