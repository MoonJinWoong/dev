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
	mIocpService->StartIocpService();
}

void LogicMain::onStart()
{
	mThreadState = true;
	//auto MaxPacketSize = m_Config.MaxPacketSize + 1;
	int MaxPacketSize = 8192;  // 이거 수정해야함
	auto pBuf = new char[MaxPacketSize];
	ZeroMemory(pBuf, sizeof(char) * MaxPacketSize);
	INT32 waitTimeMillisec = 1;


	// 로직 처리 
	// 루프 돌면서 메세지를 계속 가져옴 
	while (mThreadState)
	{
		INT8 operationType = 0;
		INT32 connectionIndex = 0;
		INT16 copySize = 0;

		auto ret = mIocpService->GetNetworkMsg(connectionIndex, pBuf, copySize);
		if (!ret)
		{
			continue;
		}
	}
}

void LogicMain::onShutDown()
{
	mThreadState = false;
}





