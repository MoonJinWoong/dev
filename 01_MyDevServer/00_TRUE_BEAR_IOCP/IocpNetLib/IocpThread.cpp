
#include "IocpThread.h"


IocpThread::IocpThread(HANDLE hThread, HANDLE hCompletionPort)
	: mThreadHandle(hThread), mCompletionPort(hCompletionPort)
{
}


IocpThread::~IocpThread()
{
	CloseHandle(mThreadHandle);
}

DWORD IocpThread::Run()
{

	while (true)
	{
		DoIocpJob();

		DoSendJob(); ///< aggregated sends
	}

	return 1;
}

void IocpThread::SetManagedSendIOClientSessionIndex(const int maxClientSessionCount, const int thisThreadIndex, const int maxThreadCount)
{
	auto averCount = (int)(maxClientSessionCount / maxThreadCount);

	m_SendIOClientSessionBeginIndex = averCount * thisThreadIndex;
	m_SendIOClientSessionEndIndex = m_SendIOClientSessionBeginIndex + averCount;

	if (thisThreadIndex == (maxThreadCount - 1))
	{
		auto restCount = (int)(maxClientSessionCount % maxThreadCount);
		m_SendIOClientSessionEndIndex += restCount;
	}
}

void IocpThread::DoIocpJob()
{
	DWORD dwTransferred = 0;
	LPOVERLAPPED overlapped = nullptr;

	ULONG_PTR completionKey = 0;

	int ret = GetQueuedCompletionStatus(mCompletionPort, &dwTransferred, (PULONG_PTR)& completionKey, &overlapped, 10);

	if (ret == 0 || dwTransferred == 0)
	{
		return;
	}
	else
	{
		std::cout << "key : " << completionKey << std::endl;

	}

}


void IocpThread::DoSendJob()
{
	

}

