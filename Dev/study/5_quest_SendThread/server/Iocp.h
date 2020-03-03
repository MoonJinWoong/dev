#pragma once
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <iostream>
#include "RemoteSession.h"


//TODO GQCSEx �Ҷ� Ȱ��ȭ �� ��
//class IocpEvents
//{
//public:
//	// GetQueuedCompletionStatus���� ������ �̺�Ʈ��
//	OVERLAPPED_ENTRY m_IoArray[MAX_EVENT_COUNT];
//	int m_eventCount;
//};


class Iocp
{
public:

	Iocp()
	{
		m_workIocp = INVALID_HANDLE_VALUE;
	}
	~Iocp()
	{
		CloseHandle(m_workIocp);
	}

	bool CreateNewIocp(unsigned int threadCnt)
	{
		m_workIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, threadCnt);
		if (!m_workIocp)
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
			m_workIocp,
			(ULONG_PTR)nullptr,
			0
		);
		if (ret != m_workIocp)
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
			m_workIocp,
			(ULONG_PTR)RemoteSession,
			0
		);
		if (ret != m_workIocp)
		{
			std::cout << "fail AddDeviceRemoteSocket " << std::endl;
			return false;
		}
		return true;
	}


	//TODO: �����ؾ���
	bool PostCompletionEvent()
	{
		return true;
	}

	//TODO: �����ؾ���
	//void GetCompletionEvents(IocpEvents& IoEvent, unsigned long timeOut)
	//{
	//	// Ȯ�� EX ���. �Ѳ����� �޾ƿ���
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
	HANDLE m_workIocp;
};
