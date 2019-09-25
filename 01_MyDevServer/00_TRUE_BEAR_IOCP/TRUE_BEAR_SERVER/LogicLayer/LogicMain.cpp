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
	int MaxPacketSize = 8192;  // �̰� �����ؾ���
	auto pBuf = new char[MaxPacketSize];
	ZeroMemory(pBuf, sizeof(char) * MaxPacketSize);
	INT32 waitTimeMillisec = 1;


	// ���� ó�� 
	// ���� ���鼭 �޼����� ��� ������ 
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





