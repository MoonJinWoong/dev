#pragma once
#include "NetPreCompile.h"



class NetworkObject;
class IocpService
{
public:
	IocpService();
	~IocpService();
	void Ready();

private:
	NetObject mNetObject;
	Iocp mIocp;
};