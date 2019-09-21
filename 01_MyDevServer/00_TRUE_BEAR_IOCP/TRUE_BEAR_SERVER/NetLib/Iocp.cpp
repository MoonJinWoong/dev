#include "Iocp.h"

Iocp::Iocp() 
{
}
Iocp::~Iocp() 
{
	CloseHandle(m_hIocp);
}

void Iocp::InitIocp(const int threadCnt)
{
	m_threadCount = threadCnt;
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_threadCount);
}

void Iocp::ResisterIocp(NetObject& netObj, void* pClient)
{
	if (!CreateIoCompletionPort(
		(HANDLE)netObj.mSock,
		m_hIocp,
		(ULONG_PTR)pClient,
		m_threadCount
	))
	{
		std::cout << "CreateIocp fail..." << std::endl;
	}
}

void Iocp::GQCS()
{

}