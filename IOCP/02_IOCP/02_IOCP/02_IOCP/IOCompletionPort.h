#pragma once
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>


// class CIOCompletionPortDlg;
#define MAX_SOCKBUF 1024	// PACKET 크기 
#define MAX_CLIENT 100		// 총 접속 클라 수 
#define MAX_WORKERTHREAD 4  // 스레드 풀에 넣을 thared 수


enum enumOperation {
	OP_RECV,
	OP_SEND
};

// WSAOVERLAPPED 구조체를 확장 시켜서 필요한 정보를 더 넣음 
struct stOverlappedEx {

	WSAOVERLAPPED m_wsaOverlaeed;	    // Overlapped I/O 구조체
	SOCKET		m_socketClient;		    // 클라 소켓
	WSABUF		m_wsaBuf;			    // Overlapped I/O 작업 버퍼
	char		m_szBuf[MAX_SOCKBUF];   // 데이터 버퍼 
	enumOperation m_eOperation;			// 작업 동작 종류
};


// 클라 정보를 담기 위한 구조체
struct stClientInfo {
	SOCKET			m_socketClient;		  // client와 연결되는 소켓
	stOverlappedEx  m_stRecvOverlappedEx; // RECV Overlapped I/O 작업을 위한 변수
	stOverlappedEx  m_stSendOverlappedEx; // SEND Overlapeed I/O 작읍을 위한 변수
	// 생성자에서 멤버 변수들을 초기화
	stClientInfo()
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
		m_socketClient = INVALID_SOCKET;
	}
};


class IOCompletionPort
{
public:
	IOCompletionPort(void);
	~IOCompletionPort(void);

	// server client common method 
	// init
	bool InitSocket();
	// close
	void CloseSocket(stClientInfo* pClientInfo, bool blsForce = false);

	// server only
	// 서버의 주소정보를 소켓과 연결시키고 접속 요청을 받기 위해 
	// 소켓을 등록하는 함수
	bool BindandListen(int nBindPort);
	// 접속 요청을 수락하고 메세지를 받아서 처리하는 함수
	bool StartServer();
	// WaitingThread Queue에서 대기할 thread들을 생성
	bool CreateWorkerThread();
	// Accept 요청을 처리하는 thread 생성
	bool CreateAccepterThread();
	// 사용하지 않는 클라이언트 정보 구조체를 반환
	stClientInfo* GetEmptyClientInfo();


	// CompletionPort객체와 소켓과 CompletionKey를
	// 연결시키는 역할을 ㅎ나다. 
	bool BindIOCompletionPort(stClientInfo* pClientInfo);
	//WSARecv Overlapped I/O 작업을 시킨다. 
	bool BindRecv(stClientInfo* pClientInfo);
	//WSASend Overlapped I/O 작읍을 시킨다. 
	bool SendMsg(stClientInfo* pClientInfo, char* pMsg, int nLen);
	//Overlapped I/O 작업에 대한 완료 통보를 받아 
	// 그에 해당하는 처리를 하는 함수 
	void WorkerThread();
	// 사용자의 접속을 받는 thread 
	void AccepterThread();

	// 메인 윈도우의 포인터를 저장
	// void SetMainDlg(CIOCompletionPortDlg* pMainDlg)
		//{ m_pMainDlg = pMainDlg; }
	//CIOCompletionPortDlg* GetMainDlg() { return m_pMainDlg; }

	// 생성된 thread 파괴 
	void DestroyThread();

private:

	// client 정보 저장 구조체 
	stClientInfo* m_pClientInfo;
	// client 접속을 받기 위한 리슨 소켓
	SOCKET m_socketListen;
	// 접속되어있는 클라 수 
	int m_nClientCnt;

	// 메인 윈도우 포인터 
	// 생략

	// 작업 스레드 핸들 - WaitingThread Queue에 들어갈 스레드 
	HANDLE m_hWorkerThread[MAX_WORKERTHREAD];
	// 접속 thread 핸들 
	HANDLE m_hAccepterThread;
	//CompletionPort 객체 핸들 
	HANDLE m_hIOCP;
	// 작업 스레드 동작 플래그 
	bool m_bWorkerRun;
	// 접속 스레드 동작 플래그 
	bool m_bAccepterRun;
	// 소켓 버퍼 
	char m_szSocketBuf[1024];

};