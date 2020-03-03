#include "LogicMain.h"
#include "LogicProcess.h"

void LogicMain::Init(c_u_Int maxClient)
{
	mRecvFuncDic = std::unordered_map<int, RECV_PKT_TYPE>();

	// 접속 들어올 때
	mRecvFuncDic[(int)PACKET_TYPE::CONNECTION] = &LogicMain::ProcConnect;

	// 접속 끊을 때
	mRecvFuncDic[(int)PACKET_TYPE::DISCONNECTION] = &LogicMain::ProcDisConnect;

	//login
	mRecvFuncDic[(int)PACKET_TYPE::CS_LOGIN] = &LogicMain::ProcLogin;
	
	//Room Enter
	mRecvFuncDic[(int)PACKET_TYPE::CS_ROOM_ENTER] = &LogicMain::ProcRoomEnter;

	
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
	// worker에서 recv 호출 될때마다 온다.
	// manager 에서 client 가져와서 큐에 집어넣음
	auto pClient = mClMgr->GetClient(unique_id);
	pClient->SetPacketProc(len, msg);
	PutUserIdx(unique_id);

}

void LogicMain::LogicThread()
{
	while (isRun)
	{
		// connet , disconnect 패킷 있는지 검사.
		auto packet = GetConnectPkt();
		if (packet.packet_type != (int)PACKET_TYPE::NONE)
		{
			ProcRecv(packet.unique_id,packet.packet_type,packet.size,packet.pData);
		}

		// 어떤 유저가 보냈는지 큐에서 인덱스 검사.
		auto packet2 = GetUserPkt();
		if (packet2.packet_type >= (int)PACKET_TYPE::CS_LOGIN)
		{
			ProcRecv(packet2.unique_id, packet2.packet_type, packet2.size, packet2.pData);
		}
	}
}

void LogicMain::ProcRecv(c_u_Int uniqueId , c_int pktType , c_u_Int size , char* pData)
{

	auto iter = mRecvFuncDic.find(pktType);
	if (iter != mRecvFuncDic.end())
	{
		(this->*(iter->second))(uniqueId, size, pData);
	}
}

PacketFrame LogicMain::GetConnectPkt()
{
	// 뺄때는 락 필요 없을거 같다 로직 스레드는 하나니까
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

	//TODO 여기서 패킷 가져올때 복수개를 가져올 수 있어야 한다. 
	auto packet = cl->GetPacketProc();
	packet.unique_id = user_id;
	return packet;
}

void LogicMain::PutUserIdx(c_u_Int unique_id)
{
	auto_lock guard(mLock);
	mUserIdQueue.push(unique_id);
}



