#include "preCompile.h"


IocpServer*  IocpServer::m_pIocpServer = NULL;


IocpServer::IocpServer()
{
	cout << "IOCP constructor complete....!" << endl;
}
IocpServer::~IocpServer()
{

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
	cPort = initConfig.sServerPort;
	cWorkerThreadCnt = initConfig.sWorkerThreadCnt;
	cWorkerThreadCnt = initConfig.sProcessThreadCnt;

	if (InitIOCP() == false)
		return false;
	if (CreateIOCP() == false)
		return false;
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

bool IocpServer::CreateThreads()
{
	// worker thread and process thread 생성

	HANDLE	hWorkerThread;
	UINT	uiThreadId;

	for (DWORD dwCount = 0; dwCount < cWorkerThreadCnt; dwCount++)
	{
		hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &CallWorkerThread,
			this, CREATE_SUSPENDED, &uiThreadId);
		if (hWorkerThread == NULL)
		{
		
			return false;
		}
		cWorkerThread[dwCount] = hWorkerThread;
		ResumeThread(hWorkerThread);
	}

	// worker랑 thread랑 분리하자.. 오류 생길거 같다.. 
	// 여기서부터 다시짜보자
	

}

//unsigned int WINAPI CallWorkerThread(LPVOID p)
//{
//	IOCPServer* pServerSock = (IOCPServer*)p;
//	pServerSock->WorkerThread();
//	return 1;
//}
//
//unsigned int WINAPI CallProcessThread(LPVOID p)
//{
//	IOCPServer* pServerSock = (IOCPServer*)p;
//	pServerSock->ProcessThread();
//	return 1;
//}
