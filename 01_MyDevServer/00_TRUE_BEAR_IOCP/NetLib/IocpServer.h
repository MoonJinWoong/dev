#pragma once

#include "PreComplie.h"




class IocpServer 
{
public:
	IocpServer(const int &threadCnt);
	~IocpServer();
	void Resister(NetTool& socket, void* userPtr);
	bool GQCS(EXOverlapped& inputEvent, int timeoutMs);

	HANDLE& getIocp() { return mhIocp; }

public:
	int				mThreadCnt;
	HANDLE			mhIocp;
	int				mEventCnt;
private:
};
