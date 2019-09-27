#pragma once
#include "NetPreCompile.h"

class Session;
class Message;
class IocpEvents;


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


	bool PQCSWorker(Session *pSession, Message *pMsg , DWORD packetSize)
	{
		auto result = PostQueuedCompletionStatus(
			m_workIocp,
			packetSize,
			reinterpret_cast<ULONG_PTR>(pSession),
			reinterpret_cast<LPOVERLAPPED>(pMsg));

		if (!result)
		{
			return false;
		}
		return true;
	}

	bool GQCS_InLogic(Session* pSession, Message* pMsg)
	{

		DWORD ioSize = 0;
		auto result = GetQueuedCompletionStatus(
			m_logicIocp,
			&ioSize,
			reinterpret_cast<PULONG_PTR>(&pSession),
			reinterpret_cast<LPOVERLAPPED*>(&pMsg),
			INFINITE);

		if (!result)
		{
			return false;
		}
		return true;
	}

	void GQCS_InWork(IocpEvents& IoEvent ,DWORD timeOut)
	{
		// Ex 버젼
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
