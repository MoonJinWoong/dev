#include "LogicMain.h"


void LogicMain::Init(c_u_Int maxClient)
{
}

bool LogicMain::Start()
{
	isRun = true;
	mLogicThread = std::thread([this]() { LogicThread(); });
	return true;
}

void LogicMain::Stop()
{

}

void LogicMain::RecvPktData(c_u_Int unique_id, c_u_Int len, char* msg)
{

}

void LogicMain::LogicThread()
{
	//while (isRun)
	//{
	//	//if (auto packetData = PopPkt(); packetData.PacketId > (UINT16)PACKET_ID::SYS_END)

	//}
}

PacketFrame LogicMain::GetConnectPkt()
{
	std::lock_guard<std::mutex> guard(mLock);

	if (mConnectQueue.empty()) 
	{
		return PacketFrame();
	}

	auto packet = mConnectQueue.front();
	mConnectQueue.pop();

	return packet;
}

void LogicMain::PutConnectPkt(PacketFrame packet)
{
	//TODO : lock-free
	std::lock_guard<std::mutex> guard(mLock);
	mConnectQueue.push(packet);
}



