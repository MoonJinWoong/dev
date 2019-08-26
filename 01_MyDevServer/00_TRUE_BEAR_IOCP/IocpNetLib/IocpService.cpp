
#include "precompile.h"




// »ý¼ºÀÚ
IocpService::IocpService(int threadCnt) : mThreadCnt(threadCnt)
{
	// create iocp 
	mIocpHanle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, mThreadCnt);
	

}


IocpService::~IocpService()
{
}
void IocpService::resister(NetworkService& sock)
{
	if (!CreateIoCompletionPort
	(
		(HANDLE)sock.mSock,
		mIocpHanle,
		0,
		mThreadCnt
	))
		cout << "[ERR] CreateIoCompletionPort error " << endl;
}
