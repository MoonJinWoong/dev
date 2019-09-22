#pragma once
#include "NetPreCompile.h"

class Session;
class Message;

class Iocp
{
public:
	// 1회의 GQCS이 최대한 꺼내올 수 있는 일의 갯수
	static const int MaxEventCount = 1000;
	static const int IOCP_THREAD_COUNT = 8;

	Iocp()
	{
		m_threadCount = IOCP_THREAD_COUNT;
		m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, IOCP_THREAD_COUNT);
	}
	~Iocp()
	{
		CloseHandle(m_hIocp);
	}

	void ResisterIocp(SOCKET& socket , void* pUser)
	{
		if (!CreateIoCompletionPort((HANDLE)socket, m_hIocp, (ULONG_PTR)pUser, m_threadCount))
		{
			std::cout << "CreateCompletionPort fail" << std::endl;
		}
	}

	bool GQCS(Message* msg , Session* session, DWORD&  ioSize)
	{
		auto result = GetQueuedCompletionStatus(
			m_hIocp,
			&ioSize,
			reinterpret_cast<PULONG_PTR>(&session),
			reinterpret_cast<LPOVERLAPPED*>(&msg),
			INFINITE);

		if (!result)
		{
			return false;
		}

		return true;
	}






public:
	int m_threadCount;
	HANDLE m_hIocp;
};
