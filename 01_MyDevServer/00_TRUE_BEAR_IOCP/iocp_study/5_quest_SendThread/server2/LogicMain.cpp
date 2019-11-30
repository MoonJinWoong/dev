#include "LogicMain.h"
#include "LogicProcess.h"

void LogicMain::Init(u_Int maxClient)
{
	mRecvFuncDic = std::unordered_map<int, RECV_PKT_TYPE>();

	// 접속 들어올 때
	mRecvFuncDic[(int)PACKET_TYPE::CONNECTION] = &LogicMain::ProcConnect;

	// 접속 끊을 때
	mRecvFuncDic[(int)PACKET_TYPE::DISCONNECTION] = &LogicMain::ProcDisConnect;

	// login
	mRecvFuncDic[(int)PACKET_TYPE::CS_LOGIN] = &LogicMain::ProcLogin;
	
	// RoomEnter
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

void LogicMain::RecvPktData(u_Int unique_id, char* pBuf , int size)
{
	// OnRecv 에서 완성되서 온다. 
	auto_lock guard(mLock);
	std::cout << "test" << sizeof(pBuf) << std::endl;
	memcpy(pBuf, pBuf, size);

	auto pHeader = (PKT_HEADER*)pBuf;
	if (pHeader->packet_len < sizeof(PKT_HEADER))
	{
		return;
	}

	// 조립
	PacketFrame packet;
	packet.packet_type = pHeader->packet_type;
	packet.size = pHeader->packet_len;
	packet.pData = pBuf;
	packet.unique_id = unique_id;

	mRecvPktQ.push(packet);
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
		if (mRecvPktQ.empty())
		{
			continue;
		}
		else
		{
			auto packet2 = mRecvPktQ.front();
			mRecvPktQ.pop();
			if (packet2.packet_type >= (int)PACKET_TYPE::CS_LOGIN)
			{
				ProcRecv(packet2.unique_id, packet2.packet_type, packet2.size, packet2.pData);
			}
		}

	}
}

void LogicMain::ProcRecv(u_Int uniqueId , c_int pktType , u_Int size , char* pData)
{
	auto iter = mRecvFuncDic.find(pktType);
	if (iter != mRecvFuncDic.end())
	{
		(this->*(iter->second))(uniqueId, size, pData);
	}
}

PacketFrame LogicMain::GetConnectPkt()
{
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
	auto_lock guard(mLock);
	mConnectQueue.push(packet);
}

//PacketFrame LogicMain::DeQueueRecvPkt()
//{
//	// 큐에 들어있는 recv 받은 유저리스트들을 불러와서 
//	// 패킷이 완성 되었으면 꺼내고 pop 한다.
//	auto_lock guard(mLock);
//	if (mRecvPktQ.empty())
//	{
//		return PacketFrame();
//	}
//
//	auto unique_id = mRecvPktQ.front();
//	mRecvPktQ.pop();
//
//	auto cl = mClMgr->GetClient(unique_id);
//	auto packet = cl->GetPacketProc();
//	
//	
//	packet.unique_id = unique_id;
//	return packet;
//}
//
//void LogicMain::EnQueueRecvPkt(u_Int unique_id)
//{
//	auto_lock guard(mLock);
//	mRecvPktQ.push(unique_id);
//}



