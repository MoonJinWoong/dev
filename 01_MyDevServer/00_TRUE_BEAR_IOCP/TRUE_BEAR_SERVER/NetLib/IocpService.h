#pragma once
#include "NetPreCompile.h"



class NetworkObject;
class IocpService
{
public:
	IocpService();
	~IocpService();
	void Ready();
	bool ProcessMsg(OUT INT8& msgType, OUT INT32& sessionIdx,
		char* buf, OUT INT16& copySize, const INT32 waitTime);

private:
	NetObject mNetObject;
	Iocp mIocp;
};