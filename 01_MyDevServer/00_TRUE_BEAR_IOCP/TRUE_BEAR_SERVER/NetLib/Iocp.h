#pragma once
#include "NetPreCompile.h"
#include "NetObject.h"
class NetObject;
class Iocp
{
public:
	Iocp()
	{
	}
	~Iocp()
	{
		CloseHandle(m_hIocp);
	}
	
	void InitIocp(const int threadCnt)
	{
		m_threadCount = threadCnt;
		m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_threadCount);
	}
	void ResisterIocp(NetObject &netObj , void* pClient)
	{
		auto ret = CreateIoCompletionPort(
						(HANDLE)netObj.mSock,
						m_hIocp,
						(ULONG_PTR)pClient,
						m_threadCount
		);
		if(ret == false)
		{
			std::cout << "CreateIocp fail..." << std::endl;
		}

	}

	void GQCS()
	{

	}

private:
	int m_threadCount;
	HANDLE m_hIocp;
};
