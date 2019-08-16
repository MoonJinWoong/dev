#pragma once

#include "PreComplie.h"

#define	MAX_BUFFER		1024
#define SERVER_PORT		8000



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
