
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


	ExOverlappedIO* context = reinterpret_cast<ExOverlappedIO*>(overlapped);

	Session* remote = context ? context->mSessionObject : nullptr;


	if (ret == 0 || dwTransferred == 0)
	{
		/// check time out first 
		if (context == nullptr && GetLastError() == WAIT_TIMEOUT) 
		{
			//std::cout << "34343434" << std::endl;

			return;
		}

		if (context->mIoType == IO_RECV || context->mIoType == IO_SEND)
		{
			std::cout << "123123123" << std::endl;

			//auto disconnectReason = static_cast<OverlappedDisconnectContext*>(context)->mDisconnectReason;
			//DeleteIoContext(context);

			///// In most cases in here: ERROR_NETNAME_DELETED(64)
			//remote->DisconnectCompletion(disconnectReason);

			return;
		}
	}

	bool completionOk = false;
	switch (context->mIoType)
	{

	case IO_ACCEPT:
		completionOk = dynamic_cast<Session*>(remote)->AcceptCompletion();
		// TODO: completionOk가 false인 경우 실패 이유가 설정 되어야 한다. remote->SetDisconnectReason
		break;

	}

}


void IocpThread::DoSendJob()
{
	

}

