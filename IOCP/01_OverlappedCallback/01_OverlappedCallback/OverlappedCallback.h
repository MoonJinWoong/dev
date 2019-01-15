#pragma once


#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <process.h>



// class COverlappedCallbackDlg;
#define MAX_SOCKBUF 1024


// Overlapped I/O �۾� ���� ����
enum enumOperation {
	OP_RECV,
	OP_SEND
};

//WSAOVERLAPPED ����ü�� Ȯ�� ���Ѽ� �ʿ��� ������ �� �־���
struct stOverlappedEx {

	WSAOVERLAPPED m_wsaOverlapped;		// Overlapped IO ����ü
	SOCKET m_socketClient;				// Ŭ���̾�Ʈ ����
	WSABUF m_wsaBuf;					// Overlapped IO�۾� ���� 
	char m_szBuf[MAX_SOCKBUF];			// ������ ���� 
	enumOperation m_eOperation;         // �۾� ���� ����
	void* m_pOverlappedCallback;        // �ݹ� �Լ� ������
};

class OverlappedCallback
{
public:
	OverlappedCallback(void);
	~OverlappedCallback(void);

	//------ ���� Ŭ�� ���� �Լ�------ 
	// ���� �ʱ�ȭ 
	bool InitSocket();
	// ���� ���� ����
	void CloseSocket(SOCKET socketClose, bool blsForce = false);
	// -------------------------------

	// ----- ������ �Լ� ---------
	// server�� �ּ������� ���ϰ� �����Ű�� ���� ��û�� �ޱ� ���� �� ������ ����ϴ� �Լ�
	bool BindandListen(int nBindPort);

	// ���� ��û�� �����ϰ� �޼����� �޾Ƽ� ó���ϴ� �Լ�
	bool StartServer();

	// accept ��û�� ó���ϴ� thread ����
	bool CreateAccepterThread();

	// WSARecv Overlapped IO �۾��� ��Ų��.
	bool BindRecv(SOCKET socket);

	// WSASend Overlapped IO �۾��� ��Ų��.
	bool SendMsg(SOCKET socket, char* pMsg, int nLen);

	// ������� ������ �޴� ������
	void AccepterThread();

	// void SetMainDlg(COverlappedCallbackDlg* pMainDlg){m_pMainDlg = pMainDlg;}
	//COverlappedCallbackDlg* GetMainDlg(){return m_pMainDlg;}

	// �����Ǿ��ִ� �����带 �ı�
	void DestroyThread();



private:

	//���� �Ǿ��ִ� Ŭ�� ��
	int			m_nClientCnt;

	// ���� ������ ������ 
	// COverlappedCallbackDlg* m_pMainDlg;

	// ���� ������ �ڵ�
	HANDLE		m_hAccepterThread;

	// ���� ������ ���� �÷���
	bool		m_bAccepterRun;
	SOCKET		m_sockListen;

	// ���� ����
	char		m_szSocketBuf[1024];
};