#pragma once
#include "NetPreCompile.h"

class Session;
class Message;
class IocpEvents;
class CustomException;

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
	}
	~Iocp()
	{
		CloseHandle(m_workIocp);
	}
	void CreateNewIocp()
	{
		m_workIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, IOCP_THREAD_COUNT);
		if (!m_workIocp)
		{
			throw CustomException("CreateNewIocp fail : ");
		}
	}

	void AddDeviceIocp(SOCKET& socket , void* pUser)
	{
		auto ret = CreateIoCompletionPort((HANDLE)socket, m_workIocp, (ULONG_PTR)pUser, 0);
		if(ret != m_workIocp)
		{
			throw CustomException("Add Device Iocp fail : ");
		}
	}


	bool PostCompletionEvent(Session *pSession, Message *pMsg , DWORD packetSize)
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



	void GetCompletionEvents(IocpEvents& IoEvent ,DWORD timeOut)
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
};
