#pragma once
#include "NetPreCompile.h"

class NetObject;
class Iocp
{
public:
	Iocp();
	~Iocp();

	void InitIocp(const int threadCnt);
	void ResisterIocp(NetObject& netObj, void* pClient);
	void GQCS();
private:
	int m_threadCount;
	HANDLE m_hIocp;
};
