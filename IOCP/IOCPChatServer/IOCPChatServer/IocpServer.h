#pragma once




class NETLIB IocpServer
{
public:
	IocpServer(void);
	virtual ~IocpServer(void);

	static IocpServer* m_pIocpServer;


	// init and start and stop 
	bool InitIOCP();
	virtual bool ServerStart(const INITCONFIG &initconfig);     // ��ӹ��� ���� �ݵ�� �̰� �������־����
	virtual bool ServerStop();      // ��ӹ��� ���� �ݵ�� �̰� �������־����
	
	void WorkerThread();
	void ProcessThread();


private:
	bool CreateIOCP();				  // workeriocp processiocp ��������
	bool CreateWorkerThread();		  // worker thread ����
	bool CreateProcessThread();


protected:
	SOCKET cListenSocket;
	
	HANDLE cWorkerThread[ MAX_WORKER_THREAD ];
	HANDLE cProcessThread[MAX_PROCESS_TRHEAD];

	HANDLE cWorkerIOCP;
	HANDLE cProcessIOCP;

	// DWORD : �÷����� ���� ���� �ʾƼ� ��.. �˳��ϰ� ��Ƽ� ��ġ�� ������?  
	DWORD cPort;
	DWORD cWorkerThreadCnt;
	DWORD cProcessThreadCnt;

	BOOL cWorkerThreadFlag;
	BOOL cProcessThreadFlag;
	


	
};