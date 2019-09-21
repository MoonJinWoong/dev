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
	mIocpService = std::make_unique<IocpService>();
}

void LogicMain::Start()
{
	mThreadState = true;
	//auto MaxPacketSize = m_Config.MaxPacketSize + 1;
	int MaxPacketSize = 8192;  // 이거 수정해야함
	auto pBuf = new char[MaxPacketSize];
	ZeroMemory(pBuf, sizeof(char) * MaxPacketSize);
	INT32 waitTimeMillisec = 1;

	while (mThreadState)
	{

		INT8 operationType = 0;
		INT32 connectionIndex = 0;
		INT16 copySize = 0;

		// 아래부터 다시 한다. 
		if (!mIocpService->ProcessMsg(
			operationType,
			connectionIndex,
			pBuf,
			copySize,
			waitTimeMillisec))
		{
			continue;
		}

	}
}

void LogicMain::ShutDown()
{
	mThreadState = false;
}





