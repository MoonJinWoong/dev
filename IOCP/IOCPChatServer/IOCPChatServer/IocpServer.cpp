#include "preCompile.h"

unsigned int WINAPI CallWorkerThread(LPVOID p);
unsigned int WINAPI CallProcessThread(LPVOID p);

IocpServer*  IocpServer::m_pIocpServer = NULL;


IocpServer::IocpServer()
{
	cout << "IOCP constructor complete....!" << endl;
}
IocpServer::~IocpServer()
{

}
unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	IocpServer* pWorkerServerSock = (IocpServer*)p;
	pWorkerServerSock->WorkerThread();

	return 1;
}

unsigned int WINAPI CallProcessThread(LPVOID p)
{
	IocpServer* pProcessServerSock = (IocpServer*)p;
	pProcessServerSock->ProcessThread();

	return 1;
}
bool IocpServer::InitIOCP()
{
	// worker thread handle 초기화					(범위기반으로...?이거 테스트해보자 )
	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
		cWorkerThread[i] = INVALID_HANDLE_VALUE;

	cWorkerIOCP   = INVALID_HANDLE_VALUE;
	cProcessIOCP  = INVALID_HANDLE_VALUE;
	cListenSocket = INVALID_SOCKET;

	WSADATA		WsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (nRet)
	{
		cout << "WSAStartup error ....!" << endl;
		return false;
	}
	
	// 이코드 나중에 삭제 
	cout << "WSAStartup success ....!" << endl;
	return true;
}

bool IocpServer::ServerStart(const INITCONFIG &initConfig)
{
	// app에서 초기화 한 후 이곳을 호출한다. 

	cPort = initConfig.sServerPort;
	cWorkerThreadCnt = initConfig.sWorkerThreadCnt;
	cProcessThreadCnt = initConfig.sProcessThreadCnt;

	if (InitIOCP() == false)
		return false;
	if (!CreateIOCP())   // workeriocp process iocp 두개 생성해주는 곳 
		return false;
	if (!CreateWorkerThread())
		return false;
	if (!CreateProcessThread())
		return false;
	if (!CreateListenSocket())
		return false;

	return true;
}

bool IocpServer::ServerStop()
{
	if (cWorkerIOCP)
	{
		cWorkerThreadFlag = false;
		for (DWORD i = 0; i < cWorkerThreadCnt; i++)
		{
			// WorkerThread에 종료 메시지를 보낸다.
			//PostQueuedCompletionStatus(cWorkerIOCP, 0, 0, NULL);
		}
		CloseHandle(cWorkerIOCP);
		cWorkerIOCP = NULL;
	}
	if (cProcessIOCP)
	{
		cProcessThreadFlag = false;
		for (DWORD i = 0; i < cProcessThreadCnt; i++)
		{
			// ProcessThread에 종료 메시지를 보낸다.
			// PostQueuedCompletionStatus(cProcessIOCP, 0, 0, NULL);
		}
		CloseHandle(cProcessIOCP);
		cProcessIOCP = NULL;
	}

	// 핸들을 닫는다.
	for (unsigned int i = 0; i < cWorkerThreadCnt; i++)
	{
		if (cWorkerThread[i] != INVALID_HANDLE_VALUE)
			CloseHandle(cWorkerThread[i]);
		cWorkerThread[i] = INVALID_HANDLE_VALUE;
	}
	// 핸들을 닫는다.
	for (unsigned int i = 0; i < cProcessThreadCnt; i++)
	{
		if (cProcessThread[i] != INVALID_HANDLE_VALUE)
			CloseHandle(cProcessThread[i]);
		cProcessThread[i] = INVALID_HANDLE_VALUE;
	}

	if (cListenSocket != INVALID_SOCKET)
	{
		closesocket(cListenSocket);
		cListenSocket = INVALID_SOCKET;
	}
	return true;
}

bool IocpServer::CreateIOCP()
{
	// worker iocp and process iocp 생성해줌.
	cWorkerIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (cWorkerIOCP == NULL)
	{
		cout << "Worker IOCP Create failed....!!" << endl;
		return false;
	}
	cProcessIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	if (cProcessIOCP == NULL)
	{
		cout << "Process IOCP Create failed....!!" << endl;
		return false;
	}
	return true;
}

bool IocpServer::CreateWorkerThread()
{
	// worker thread 생성

	HANDLE	hWorkerThread;
	UINT	uiThreadId;

	
	for (int dwCount = 0; dwCount < cWorkerThreadCnt; dwCount++)
	{
		hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &CallWorkerThread,
			this, CREATE_SUSPENDED, &uiThreadId);
		
		if (hWorkerThread == NULL)
		{
			cout << "CreateWorkerThread failed....!!!" << endl;
			return false;
		}

		// 실제 스레드 주소 
		cWorkerThread[dwCount] = hWorkerThread;

		 ResumeThread(hWorkerThread);

	}


	cout << "** WorkerThread " << cWorkerThreadCnt << "개 생성...." << endl;
	return true;
}

bool IocpServer::CreateProcessThread()
{
	HANDLE	hProcessThread;
	UINT	uiThreadId;

	// create worker thread.
	for (DWORD dwCount = 0; dwCount < cProcessThreadCnt; dwCount++)
	{
		//cout << "asdfasfasdasdasdf" << endl;
		hProcessThread = (HANDLE)_beginthreadex(NULL, 0, &CallProcessThread,
			this, CREATE_SUSPENDED, &uiThreadId);
		if (hProcessThread == NULL)
		{
			cout << "CreateProcessThread failed....!!!" << endl;
			return false;
		}
		cProcessThread[dwCount] = hProcessThread;
		ResumeThread(hProcessThread);
		SetThreadPriority(hProcessThread, THREAD_PRIORITY_TIME_CRITICAL);
	}

	cout << "** Process Thread " << cProcessThreadCnt << "개 생성...." << endl;
	return true;
}

void IocpServer::WorkerThread()
{
	BOOL					bSuccess = false;
	LPOVERLAPPED			lpOverlapped = NULL;
	ConnectionManager*			lpConnection = NULL;
	LPOVERLAPPED_EX			lpOverlappedEx = NULL;
	DWORD					dwIoSize = 0;


	while (cWorkerThreadFlag)
	{
		cout << "worker thread call success... ! " << endl;
	}
	
}

void IocpServer::ProcessThread()
{
	//cout << "process thread call success... ! " << endl;
}

bool IocpServer::CreateListenSocket()
{
	SOCKADDR_IN	si_addr;
	int			nRet;
	int			nZero = 0;


	cListenSocket = WSASocket(AF_INET, SOCK_STREAM,
		IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == cListenSocket)
	{
		cout << "listen socket call error ...!!" << endl;
		return false;
	}
	else {
		cout << "listen socket Create Success...!!!!" << endl;
	}

	// bind listen socket with si_addr struct.
	si_addr.sin_family = AF_INET;
	si_addr.sin_port = htons(cPort);
	si_addr.sin_addr.s_addr = htonl(INADDR_ANY);



	// std 랑 충돌해서 :: 붙여줘야함.
	int ret = ::bind(cListenSocket, (struct sockaddr *) &si_addr, sizeof(si_addr));
	if (ret == SOCKET_ERROR)
		cout << "bind error...!!1" << endl;


	// start listening..
	nRet = listen(cListenSocket, 50);

	if (SOCKET_ERROR == nRet)
	{
		cout << "listen() call error...!" << endl;
		return false;
	}

	HANDLE hIOCPHandle;
	hIOCPHandle = CreateIoCompletionPort((HANDLE)cListenSocket,
		cWorkerIOCP, (DWORD)0, 0);

	if (NULL == hIOCPHandle || cWorkerIOCP != hIOCPHandle)
	{
		cout << "createIoCompletionPort call failed....!" << endl;
		return false;
	}
	return true;
}