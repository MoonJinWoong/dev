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


	// ���� ���鼭 �޼����� ��� ������ 
	while (mThreadState)
	{
		INT8 operationType = 0;
		INT32 connectionIndex = 0;
		INT16 copySize = 0;

		//���� �Լ� �����ϱ�(���� �����Ҵ� ��� �Ǿ)

		//WorkThread�� �Լ����� �ҷ��ͼ� ó���Ѵ�.
		//if (!mIocpService->getNetworkMessage(operationType,
		//	connectionIndex,
		//	pBuf,
		//	copySize
		//	))
		//{
		//	continue;
		//}

		//auto msgType = (MsgType)operationType;

		//switch (msgType)
		//{
		//case MsgType::Session:
		//	std::cout<<"Accept Session :" <<connectionIndex<<std::endl;
		//	break;
		//case MsgType::Close:
		//	//m_pPacketManager->ClearConnectionInfo(connectionIndex);
		//	break;
		//case MsgType::OnRecv:
		//	//m_pPacketManager->ProcessRecvPacket(connectionIndex, pBuf, copySize);
		//	break;
		//}


	}
}

void LogicMain::onShutDown()
{
	mThreadState = false;
}





