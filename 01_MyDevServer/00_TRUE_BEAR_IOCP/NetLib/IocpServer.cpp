
#include "IocpServer.h"
#include <process.h>
#include <iostream>

using namespace std;



IocpServer::IocpServer(const int &threadCnt)
{
	mThreadCnt = threadCnt;
	mhIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, mThreadCnt);
	cout << "[INFO] thread count : " << mThreadCnt << endl;
	cout << "[INFO] test : " << mhIocp << endl;
}


IocpServer::~IocpServer()
{
	CloseHandle(mhIocp);
}

void IocpServer::Resister(NetTool& netObj, void* userPtr)
{
	if (!CreateIoCompletionPort
	(
		(HANDLE)netObj.mhSock,
		mhIocp, 
		(ULONG_PTR)userPtr,
		mThreadCnt))
	{
		cout << "CreateIOCP is Fail" << endl;
	}
}

bool IocpServer::GQCS(EXOverlapped& inputEvent, int timeoutMs)
{

	// Overlapped I/O 작업에서 전송된 데이터 크기
	DWORD	recvBytes;

	DWORD	dwFlags = 0;

	int key;

	bool ret = GetQueuedCompletionStatus(
		mhIocp,
		&recvBytes,
		(PULONG_PTR)& key,
		(LPOVERLAPPED*)& inputEvent,
		timeoutMs);

	if (!ret)
	{
		// 들어온 event가 없다.
		return false;
	}
	else
	{
		cout << "recvByte:" << recvBytes <<endl;
		return true;
	}
}





