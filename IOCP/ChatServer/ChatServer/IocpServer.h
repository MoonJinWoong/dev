#pragma once


class IocpServer
{
private:
	HANDLE cWorkerIOCP;
	SOCKET cSocket;
	HANDLE cWorkerThread[MAX_WORKER_THREAD];

public:
	IocpServer();
	~IocpServer();

	bool start(INITCONFIG initconfig);
	
	bool InitializeSocket(INITCONFIG initconfig);
	void AcceptThread();
	void WorkerThread();
	void makeThreads();


	void ProcessPacket(int cl, unsigned char *packet);

public:
	int cWorkerThreadCount;
	
};