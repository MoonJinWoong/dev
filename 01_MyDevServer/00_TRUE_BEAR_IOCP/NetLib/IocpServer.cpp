
#include "IocpServer.h"
#include <process.h>
#include <iostream>

using namespace std;



IocpServer::IocpServer(const int &threadCnt)
{
	mThreadCnt = threadCnt;
	mhIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, mThreadCnt);
	cout << "[INFO] thread count : " << mThreadCnt << endl;
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



