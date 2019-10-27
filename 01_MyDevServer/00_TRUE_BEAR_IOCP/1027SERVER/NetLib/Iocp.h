#pragma once
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>


class Session;
class Message;
class IocpEvents;
class CustomException;

class IocpEvents
{
public:
	// GetQueuedCompletionStatus으로 꺼내올 이벤트들
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
		m_workIocp = INVALID_HANDLE_VALUE;
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
			std::cout << "fail CreateNewIocp " << std::endl;
		}
	}
	void AddDeviceListenSocket(SOCKET listenSock)
	{
		auto ret = CreateIoCompletionPort(
			(HANDLE)listenSock,
			m_workIocp,
			(ULONG_PTR)nullptr,
			0
		);
		if (ret != m_workIocp)
		{
			std::cout << "fail AddDeviceListenSocket " << std::endl;
		}
	}
	void AddDeviceRemoteSocket(Session* pSession)
	{
		auto ret = CreateIoCompletionPort(
			(HANDLE)pSession->GetRemoteSocket(),
			m_workIocp,
			(ULONG_PTR)pSession,
			0
		);
		if(ret != m_workIocp)
		{
			std::cout << "fail AddDeviceRemoteSocket " << std::endl;
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
		// EX 사용. 한꺼번에 받아오자
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
