#pragma once
#include "NetPreCompile.h"

class Session;
class Message;
class IocpEvents;



// IOCP의 GetQueuedCompletionStatus로 받은 I/O 완료신호들
class IocpEvents
{
public:
	// GetQueuedCompletionStatus으로 꺼내온 이벤트들
	OVERLAPPED_ENTRY m_IoArray[MAX_EVENT_COUNT];
	int m_eventCount;
};


class Iocp
{
public:
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
		CloseHandle(m_logicIocp);
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
		// 안씀
		return true;
	}

	void GQCSInWorker(IocpEvents& IoEvent ,DWORD timeOut)
	{
		// Ex 버젼!
		bool ret = GetQueuedCompletionStatusEx(
			m_workIocp, 
			IoEvent.m_IoArray,
			MAX_EVENT_COUNT, 
			(ULONG*)& IoEvent.m_eventCount,
			timeOut,
			FALSE
		);

		if (!ret)
		{
			IoEvent.m_eventCount = 0;
		}

	}
public:
	int m_threadCount;
	HANDLE m_workIocp;
	HANDLE m_logicIocp;
};
