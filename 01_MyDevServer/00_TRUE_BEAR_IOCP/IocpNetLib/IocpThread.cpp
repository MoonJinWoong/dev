
#include "IocpThread.h"


IOThread::IOThread(HANDLE hThread, HANDLE hCompletionPort)
	: mThreadHandle(hThread), mCompletionPort(hCompletionPort)
{
}


IOThread::~IOThread()
{
	CloseHandle(mThreadHandle);
}

DWORD IOThread::Run()
{

	while (true)
	{
		DoIocpJob();

		DoSendJob(); ///< aggregated sends
	}

	return 1;
}

void IOThread::SetManagedSendIOClientSessionIndex(const int maxClientSessionCount, const int thisThreadIndex, const int maxThreadCount)
{

}

void IOThread::DoIocpJob()
{


}


void IOThread::DoSendJob()
{
	

}

