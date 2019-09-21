#pragma once

#include "..//NetLib/IocpService.h"

class LogicMain
{
public:
	LogicMain();
	~LogicMain();
	void Init();
	void Start();
	void ShutDown();
	void Run();

private:
	bool mThreadState;

	// net object 
	std::unique_ptr<IocpService> mIocpService;

};