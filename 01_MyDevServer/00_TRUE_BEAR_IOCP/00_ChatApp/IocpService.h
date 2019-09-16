#pragma once
#include "..//01_NetLibrary/preComplie.h"

class IocpService
{
public:
	IocpService();
	~IocpService();
	void ReadyIocp();
	
	
	bool CreateThreads();
	void IoThread();

	void IocpServiceShutDown();

public:
	Iocp mIocp;
	std::vector<std::unique_ptr<std::thread>> mIoThread;

};


