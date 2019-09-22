#pragma once

#include "..//NetLib/IocpService.h"

class LogicMain
{
public:
	LogicMain();
	~LogicMain();
	void Init();
	void onStart();
	void onShutDown();


	IocpService* GetIOCPServer() { return  mIocpService.get(); }

private:
	bool mThreadState;

	// net object 
	std::unique_ptr<IocpService> mIocpService;

};