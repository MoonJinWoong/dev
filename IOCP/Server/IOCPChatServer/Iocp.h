#pragma once


class Iocp
{
private:
	int			cPort;
	int			cSendBufSize;
	int			cRecvBufSize;
	int			cWorkerThreadCnt;

	WSADATA		cWsadata;     // Init���� 
	HANDLE		cIocpHandle;   //Init ���� �ʱ�ȭ
	HANDLE		cWorkerThreadHandle;   // CreateThreads()���� ����
	HANDLE		cAcceptThreadHandle;

	SOCKET		cListenSock;       // BindAndListen���� ����
	SOCKET		cServerSock;       // ��ſ� ���� ����
	SOCKET		cClientSock[MAX_USER];

	SOCKADDR_IN	cClientaddr;

	vector<thread *> workerThreads;

public:
	Iocp();
	~Iocp();
	BOOL StartServer();
	BOOL InitIOCP(const INITCONFIG & initconfig);
	BOOL BindAndListen();
	void CreateThreads();


	void WorkerThread();
	void AcceptThread();


	void CloseServer();
};