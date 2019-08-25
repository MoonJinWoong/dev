#pragma once

#include "PreComplie.h"


class IocpEvents;

class IocpServer 
{
public:
	IocpServer(const int &threadCnt);
	~IocpServer();
	void Resister(NetTool& socket, void* userPtr);
	void RunGQCS(IocpEvents& inputEvent, int waitTime);

	HANDLE& getIocp() { return mhIocp; }

public:
	int				mThreadCnt;
	HANDLE			mhIocp;
	int				mEventCnt;

	static const int MaxEventCount = 1000;
private:
};

class IocpEvents
{
public:
	OVERLAPPED_ENTRY mEvents[IocpServer::MaxEventCount];
	int mEventCount;
};
