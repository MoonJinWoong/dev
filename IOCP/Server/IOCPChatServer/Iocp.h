#pragma once


class Iocp
{
private:
	int			cPort;
	int			cSendBufSize;
	int			cRecvBufSize;
	int			cWorkerThreadCnt;

	WSADATA		cWsadata;     // Init에서 
	HANDLE		cIocpHandle;   //Init 에서 초기화
	HANDLE		cWorkerThreadHandle;   // CreateThreads()에서 해줌
	HANDLE		cAcceptThreadHandle;

	SOCKET		cListenSock;       // BindAndListen에서 해줌
	SOCKET		cServerSock;       // 통신에 사용될 소켓
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