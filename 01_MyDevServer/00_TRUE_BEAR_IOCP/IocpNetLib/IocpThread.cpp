
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
		std::cout << "asdfasdf" << std:: endl;
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


}


void IocpThread::DoSendJob()
{
	

}

