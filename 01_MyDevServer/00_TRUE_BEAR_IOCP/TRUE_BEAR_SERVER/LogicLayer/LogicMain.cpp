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


	// 루프 돌면서 메세지를 계속 가져옴 
	while (mThreadState)
	{
		INT8 operationType = 0;
		INT32 connectionIndex = 0;
		INT16 copySize = 0;

		//여기 함수 개선하기(위의 동적할당 계속 되어서)

		//WorkThread의 함수들을 불러와서 처리한다.
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





