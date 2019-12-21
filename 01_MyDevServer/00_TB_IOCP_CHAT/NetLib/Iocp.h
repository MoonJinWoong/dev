#pragma once
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <iostream>
#include "RemoteSession.h"
#include "easylogging++.h"



class Iocp
{
public:

	class IocpEvents
	{
	public:
		OVERLAPPED_ENTRY m_IoArray[IOCP_EVENT_COUNT];
		int m_eventCount;
	};
	 
	~Iocp()
	{
		CloseHandle(mIocp);
	}

	bool CreateNewIocp(unsigned int threadCnt)
	{
		mIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, threadCnt);
		if (!mIocp)
		{
			LOG(ERROR) << "Iocp::CreateNewIocp";
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
			LOG(ERROR) << "Iocp::AddDeviceListenSocket";
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
			LOG(ERROR) << "Iocp::AddDeviceRemoteSocket";
			return false;
		}
		return true;
	}

	void GQCSEx(Iocp::IocpEvents& IoEvent, unsigned long timeOut)
	{
		bool ret = GetQueuedCompletionStatusEx(
			mIocp,
			IoEvent.m_IoArray,
			IOCP_EVENT_COUNT,	//한번에 가져올 이벤트 양
			(ULONG*)&IoEvent.m_eventCount,
			timeOut,
			FALSE
		);

		if (!ret)
		{
			IoEvent.m_eventCount = 0;
		}
	}

	HANDLE& GetIocp()  { return mIocp; }
private:
	HANDLE mIocp = INVALID_HANDLE_VALUE;
};
