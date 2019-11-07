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
		// connet , disconnect ��Ŷ �ִ��� �˻�.
		auto packet = GetConnectPkt();
		if (packet.packet_type != (unsigned short)PACKET_TYPE::NONE)
		{
			ProcRecv(packet);
		}

		// � ������ ���´��� ť���� �ε����� �̾ƿͼ� ó���Ѵ�.
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
	// ������ �� �ʿ� ������ ���� ���� ������� �ϳ��ϱ�
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
	// ������ �� �ʿ� ������ ����. ������ �ϳ�
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



