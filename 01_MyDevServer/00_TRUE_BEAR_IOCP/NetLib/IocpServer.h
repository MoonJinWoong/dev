#pragma once

#include "PreComplie.h"




class SessionMgr;

class IocpServer 
{
public:
	IocpServer(const int &threadCnt);
	~IocpServer();
	void Resister(NetTool& socket, void* userPtr);

private:
	int				mThreadCnt;
	HANDLE			mhIocp;

private:
};
