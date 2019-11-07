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
	isRun = false;
	if (mLogicThread.joinable())
	{
		mLogicThread.join();
	}
}

void LogicMain::RecvPktData(c_u_Int unique_id, c_u_Int len, char* msg)
{

}

void LogicMain::LogicThread()
{
	while (isRun)
	{
		// connet , disconnect 패킷 있는지 검사.
		auto packet = GetConnectPkt();
		if (packet.packet_type != (unsigned short)PACKET_TYPE::NONE)
		{
			ProcRecv(packet);
		}

		// 어떤 유저가 보냈는지 큐에서 인덱스를 뽑아와서 처리한다.
		auto packet2 = GetUserIdx();
		if (packet2.packet_type >= (unsigned short)PACKET_TYPE::SC_LOGIN)
		{
			ProcRecv(packet2);
		}
	}
}

void LogicMain::ProcRecv(PacketFrame& packet)
{

}

PacketFrame LogicMain::GetConnectPkt()
{
	// 뺄때는 락 필요 없을거 같다 로직 스레드는 하나니까
	//auto_lock guard(mLock);
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
	auto_lock guard(mLock);
	mConnectQueue.push(packet);
}

PacketFrame LogicMain::GetUserIdx()
{
	// 뺄때는 락 필요 없을거 같다. 로직은 하나
	//auto_lock guard(mLock);
	if (mUserIdQueue.empty())
	{
		return PacketFrame();
	}

	auto user_id = mUserIdQueue.front();
	mUserIdQueue.pop();


	//auto pUser = mUserManager->GetUserByConnIdx(user_id);
	//auto packetData = pUser->GetPacket();
	//packetData.unique_id = user_id;
	//return packetData;
}

void LogicMain::PutUserIdx(c_u_Int unique_id)
{
	auto_lock guard(mLock);
	mUserIdQueue.push(unique_id);
}



