#pragma once
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <iostream>
#include "RemoteSession.h"


//TODO GQCSEx 할때 활성화 할 것
//class IocpEvents
//{
//public:
//	// GetQueuedCompletionStatus으로 꺼내올 이벤트들
//	OVERLAPPED_ENTRY m_IoArray[MAX_EVENT_COUNT];
//	int m_eventCount;
//};


class Iocp
{
public:

	Iocp()
	{
		mIocp = INVALID_HANDLE_VALUE;
	}
	~Iocp()
	{
		CloseHandle(mIocp);
	}

	bool CreateNewIocp(unsigned int threadCnt)
	{
		mIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, threadCnt);
		if (!mIocp)
		{
			std::cout << "fail CreateNewIocp " << std::endl;
			return false;
		}
		return true;
	}

	bool AddDeviceListenSocket(SOCKET listenSock)
	{
		auto ret = CreateIoCompletionPort(
			(HANDLE)listenSock,
			mIocp,
			(ULONG_PTR)nullptr,
			0
		);
		if (ret != mIocp)
		{
			std::cout << "fail AddDeviceListenSocket " << std::endl;
			return false;
		}
		return true;
	}
	bool AddDeviceRemoteSocket(RemoteSession* RemoteSession)
	{
		auto ret = CreateIoCompletionPort(
			(HANDLE)RemoteSession->GetSock(),
			mIocp,
			(ULONG_PTR)RemoteSession,
			0
		);
		if (ret != mIocp)
		{
			std::cout << "fail AddDeviceRemoteSocket " << std::endl;
			return false;
		}
		return true;
	}


	//TODO: 구현해야함
	bool PostCompletionEvent()
	{
		return true;
	}

	//TODO: 구현해야함
	//void GetCompletionEvents(IocpEvents& IoEvent, unsigned long timeOut)
	//{
	//	// 확장 EX 사용. 한꺼번에 받아오자
	//	bool ret = GetQueuedCompletionStatusEx(
	//		m_workIocp,
	//		IoEvent.m_IoArray,
	//		MAX_EVENT_COUNT,
	//		(ULONG*)&IoEvent.m_eventCount,
	//		timeOut,
	//		FALSE
	//	);

	//	if (!ret)
	//	{
	//		IoEvent.m_eventCount = 0;
	//	}
	//}
public:
	HANDLE mIocp;
};
