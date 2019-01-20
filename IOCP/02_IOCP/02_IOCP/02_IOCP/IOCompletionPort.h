#pragma once
#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>


// class CIOCompletionPortDlg;
#define MAX_SOCKBUF 1024	// PACKET ũ�� 
#define MAX_CLIENT 100		// �� ���� Ŭ�� �� 
#define MAX_WORKERTHREAD 4  // ������ Ǯ�� ���� thared ��


enum enumOperation {
	OP_RECV,
	OP_SEND
};

// WSAOVERLAPPED ����ü�� Ȯ�� ���Ѽ� �ʿ��� ������ �� ���� 
struct stOverlappedEx {

	WSAOVERLAPPED m_wsaOverlaeed;	    // Overlapped I/O ����ü
	SOCKET		m_socketClient;		    // Ŭ�� ����
	WSABUF		m_wsaBuf;			    // Overlapped I/O �۾� ����
	char		m_szBuf[MAX_SOCKBUF];   // ������ ���� 
	enumOperation m_eOperation;			// �۾� ���� ����
};


// Ŭ�� ������ ��� ���� ����ü
struct stClientInfo {
	SOCKET			m_socketClient;		  // client�� ����Ǵ� ����
	stOverlappedEx  m_stRecvOverlappedEx; // RECV Overlapped I/O �۾��� ���� ����
	stOverlappedEx  m_stSendOverlappedEx; // SEND Overlapeed I/O ������ ���� ����
	// �����ڿ��� ��� �������� �ʱ�ȭ
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
	// ������ �ּ������� ���ϰ� �����Ű�� ���� ��û�� �ޱ� ���� 
	// ������ ����ϴ� �Լ�
	bool BindandListen(int nBindPort);
	// ���� ��û�� �����ϰ� �޼����� �޾Ƽ� ó���ϴ� �Լ�
	bool StartServer();
	// WaitingThread Queue���� ����� thread���� ����
	bool CreateWorkerThread();
	// Accept ��û�� ó���ϴ� thread ����
	bool CreateAccepterThread();
	// ������� �ʴ� Ŭ���̾�Ʈ ���� ����ü�� ��ȯ
	stClientInfo* GetEmptyClientInfo();


	// CompletionPort��ü�� ���ϰ� CompletionKey��
	// �����Ű�� ������ ������. 
	bool BindIOCompletionPort(stClientInfo* pClientInfo);
	//WSARecv Overlapped I/O �۾��� ��Ų��. 
	bool BindRecv(stClientInfo* pClientInfo);
	//WSASend Overlapped I/O ������ ��Ų��. 
	bool SendMsg(stClientInfo* pClientInfo, char* pMsg, int nLen);
	//Overlapped I/O �۾��� ���� �Ϸ� �뺸�� �޾� 
	// �׿� �ش��ϴ� ó���� �ϴ� �Լ� 
	void WorkerThread();
	// ������� ������ �޴� thread 
	void AccepterThread();

	// ���� �������� �����͸� ����
	// void SetMainDlg(CIOCompletionPortDlg* pMainDlg)
		//{ m_pMainDlg = pMainDlg; }
	//CIOCompletionPortDlg* GetMainDlg() { return m_pMainDlg; }

	// ������ thread �ı� 
	void DestroyThread();

private:

	// client ���� ���� ����ü 
	stClientInfo* m_pClientInfo;
	// client ������ �ޱ� ���� ���� ����
	SOCKET m_socketListen;
	// ���ӵǾ��ִ� Ŭ�� �� 
	int m_nClientCnt;

	// ���� ������ ������ 
	// ����

	// �۾� ������ �ڵ� - WaitingThread Queue�� �� ������ 
	HANDLE m_hWorkerThread[MAX_WORKERTHREAD];
	// ���� thread �ڵ� 
	HANDLE m_hAccepterThread;
	//CompletionPort ��ü �ڵ� 
	HANDLE m_hIOCP;
	// �۾� ������ ���� �÷��� 
	bool m_bWorkerRun;
	// ���� ������ ���� �÷��� 
	bool m_bAccepterRun;
	// ���� ���� 
	char m_szSocketBuf[1024];

};