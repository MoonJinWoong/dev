#pragma once


class IocpServer
{
private:
	SOCKET cSocket;

	HANDLE cWorkerIOCP;
	HANDLE cWorkerThread[MAX_WORKER_THREAD];
	HANDLE cIOCP;
	
	sClientInfo* cClientInfo;
public:
	IocpServer();
	~IocpServer();

	bool start(INITCONFIG initconfig);
	bool InitializeSocket(INITCONFIG initconfig);
	bool BindAndIOPort(sClientInfo * getClientInfo);
	bool BindRecv(sClientInfo* recvClientInfo);
	bool SendMsg(sClientInfo*, char*msg, int len);
		
	void AcceptThread();
	void WorkerThread();
	void makeThreads();


	void ProcessPacket(int cl, unsigned char *packet);

	sClientInfo* GetEmptyClientInfo();
public:
	int cWorkerThreadCount;
	int cClientCnt;
};