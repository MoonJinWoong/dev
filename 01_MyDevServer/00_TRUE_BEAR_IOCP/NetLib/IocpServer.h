#pragma once

#include "PreComplie.h"





struct stSOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int				recvBytes;
	int				sendBytes;
};


class IocpServer :public NetBase
{


public:
	IocpServer();
	~IocpServer();

	// ���� ��� �� ���� ���� ����
	bool InitAndCreateSocket();
	bool Run();

	// ���� ���� �� ���� �Ŵ����� ��� 
	void ResiterSession(SOCKET acceptedSocket, SOCKADDR_IN addrInfo);

	// �۾� ������ ����
	//bool CreateWorkerThread();

	// �۾� ������
	void AcceptThread();
	void WorkerThread();

	void CloseServer();



private:
	stSOCKETINFO*   m_pSocketInfo;		// ���� ����
	SOCKET			m_listenSocket;		// ���� ���� ����
	HANDLE			m_hIOCP;			// IOCP ��ü �ڵ�
	bool			m_bAccept;			// ��û ���� �÷���
	bool			m_bWorkerThread;	// �۾� ������ ���� �÷���
	HANDLE* m_pWorkerHandle;	// �۾� ������ �ڵ�

private:
};
