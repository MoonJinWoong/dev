#pragma once
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <iostream>
#include "RemoteSession.h"



class Iocp
{
public:

	class IocpEvents
	{
	public:
		OVERLAPPED_ENTRY m_IoArray[1000]; //TODO 배열안의 인덱스 따로 처리
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
	
	bool PQCS()
	{
		return true;
	}
	
	void GQCSEx(Iocp::IocpEvents& IoEvent, unsigned long timeOut)
	{
		bool ret = GetQueuedCompletionStatusEx(
			mIocp,
			IoEvent.m_IoArray,
			1000,	//한번에 가져올 이벤트 양
			(ULONG*)&IoEvent.m_eventCount,
			timeOut,
			FALSE
		);

		if (!ret)
		{
			IoEvent.m_eventCount = 0;
		}
	}
public:
	HANDLE mIocp = INVALID_HANDLE_VALUE;
};
