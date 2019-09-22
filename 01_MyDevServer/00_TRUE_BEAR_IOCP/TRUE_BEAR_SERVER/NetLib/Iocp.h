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
		m_workIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, IOCP_THREAD_COUNT);
		m_logicIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	}
	~Iocp()
	{
		CloseHandle(m_workIocp);
	}

	void ResisterIocp(SOCKET& socket , void* pUser)
	{
		if (!CreateIoCompletionPort((HANDLE)socket, m_workIocp, (ULONG_PTR)pUser, m_threadCount))
		{
			std::cout << "CreateCompletionPort fail" << std::endl;
		}
	}

	bool GQCS(Message* msg, Session* session, DWORD& ioSize)
	{
		return true;
	}
public:
	int m_threadCount;
	HANDLE m_workIocp;
	HANDLE m_logicIocp;
};
