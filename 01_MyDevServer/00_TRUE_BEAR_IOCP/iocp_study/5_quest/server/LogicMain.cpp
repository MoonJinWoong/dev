#include "LogicMain.h"
#include "LogicProcess.h"

void LogicMain::Init(c_u_Int maxClient)
{
	mRecvFuncDic = std::unordered_map<int, RECV_PKT_TYPE>();

	// ���� ���� ��
	mRecvFuncDic[(int)PACKET_TYPE::CONNECTION] = &LogicMain::ProcConnect;

	// ���� ���� ��
	mRecvFuncDic[(int)PACKET_TYPE::DISCONNECTION] = &LogicMain::ProcDisConnect;

	//login
	mRecvFuncDic[(int)PACKET_TYPE::CS_LOGIN] = &LogicMain::ProcLogin;
	
	
	mClMgr = new ClientManager;
	mClMgr->Init(maxClient);
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
	// manager ���� client �����ͼ� ť�� �������
	auto pClient = mClMgr->GetClient(unique_id);
	pClient->SetPacketAssemble(len, msg);
	PutUserIdx(unique_id);

}

void LogicMain::LogicThread()
{
	while (isRun)
	{
		// connet , disconnect ��Ŷ �ִ��� �˻�.
		auto packet = GetConnectPkt();
		if (packet.packet_type != (u_Short)PACKET_TYPE::NONE)
		{
			ProcRecv(packet.unique_id,packet.packet_type,packet.size,packet.pData);
		}

		// � ������ ���´��� ť���� �ε��� �˻�.
		auto packet2 = GetUserPkt();
		if (packet2.packet_type >= (u_Short)PACKET_TYPE::CS_LOGIN)
		{
			ProcRecv(packet2.unique_id, packet2.packet_type, packet2.size, packet2.pData);
		}
	}
}
//void PacketManager::ProcessRecvPacket(const unsigned int clientIndex_, const unsigned short packetId_, const unsigned short packetSize_, char* pPacket_)

void LogicMain::ProcRecv(c_u_Int uniqueId , c_u_Short pktType , c_u_Int size , char* pData)
{

	auto iter = mRecvFuncDic.find(pktType);
	if (iter != mRecvFuncDic.end())
	{
		(this->*(iter->second))(uniqueId, size, pData);
	}

	std::cout << "ProcRecv******" << std::endl;
}

PacketFrame LogicMain::GetConnectPkt()
{
	// ������ �� �ʿ� ������ ���� ���� ������� �ϳ��ϱ�
	auto_lock guard(mLock);
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

PacketFrame LogicMain::GetUserPkt()
{
	auto_lock guard(mLock);
	if (mUserIdQueue.empty())
	{
		return PacketFrame();
	}

	auto user_id = mUserIdQueue.front();
	mUserIdQueue.pop();

	auto cl = mClMgr->GetClient(user_id);

	//TODO ���⼭ ��Ŷ �����ö� �������� ������ �� �־�� �Ѵ�. 
	auto packet = cl->GetPacketAssemble();
	packet.unique_id = user_id;
	return packet;
}

void LogicMain::PutUserIdx(c_u_Int unique_id)
{
	auto_lock guard(mLock);
	mUserIdQueue.push(unique_id);
}



