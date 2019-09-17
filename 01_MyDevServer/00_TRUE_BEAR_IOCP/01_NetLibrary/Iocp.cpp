#include "stdafx.h"
#include "Iocp.h"
#include "NetworkObject.h"
#include "Exception.h"


Iocp::Iocp()
{
	m_threadCount = IOCP_THREAD_COUNT;
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, IOCP_THREAD_COUNT);
}

Iocp::~Iocp()
{
	CloseHandle(m_hIocp);
}

void Iocp::ResisterIocp(NetworkObject& socket, void* userPtr)
{
	if (!CreateIoCompletionPort((HANDLE)socket.m_fd, m_hIocp, (ULONG_PTR)userPtr, m_threadCount))
		throw Exception("IOCP add failed!");
}

void Iocp::GQCS(IocpEvents &output, int timeoutMs)
{
	 BOOL r = GetQueuedCompletionStatusEx(m_hIocp, output.m_events, MaxEventCount, (ULONG*)&output.m_eventCount, timeoutMs, FALSE);
	 if (!r)
	 {
		 output.m_eventCount = 0;
	 }
}
