#pragma once




class NETLIB IocpServer
{
public:
	IocpServer(void);
	virtual ~IocpServer(void);

	static IocpServer* m_pIocpServer;


	// init and start and stop 
	bool InitIOCP();
	virtual bool ServerStart(const INITCONFIG &initconfig);     // 상속받은 놈은 반드시 이걸 정의해주어야함
	virtual bool ServerStop();      // 상속받은 놈은 반드시 이걸 정의해주어야함
	
	void WorkerThread();
	void ProcessThread();


private:
	bool CreateIOCP();				  // workeriocp processiocp 생성해줌
	bool CreateWorkerThread();		  // worker thread 생성
	bool CreateProcessThread();


protected:
	SOCKET cListenSocket;
	
	HANDLE cWorkerThread[ MAX_WORKER_THREAD ];
	HANDLE cProcessThread[MAX_PROCESS_TRHEAD];

	HANDLE cWorkerIOCP;
	HANDLE cProcessIOCP;

	// DWORD : 플랫폼에 구애 받지 않아서 씀.. 넉넉하게 잡아서 넘치진 않을듯?  
	DWORD cPort;
	DWORD cWorkerThreadCnt;
	DWORD cProcessThreadCnt;

	BOOL cWorkerThreadFlag;
	BOOL cProcessThreadFlag;
	


	
};