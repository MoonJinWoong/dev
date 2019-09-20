#include "LogicMain.h"


LogicMain::LogicMain()
{
	mThreadState = false;
}

LogicMain::~LogicMain()
{

}

void LogicMain::Init()
{
	mNetworkService = std::make_unique<IocpService>();

}

void LogicMain::Start()
{
	mThreadState = true;
	while (mThreadState)
	{

	}
}

void LogicMain::ShutDown()
{
	mThreadState = false;
}

