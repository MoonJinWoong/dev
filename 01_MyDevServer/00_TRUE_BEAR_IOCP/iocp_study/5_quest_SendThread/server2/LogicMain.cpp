#include "LogicMain.h"
#include "LogicProcess.h"

void LogicMain::Init(unsigned int maxClient)
{
	mRecvFuncDic = std::unordered_map<int, RECV_PKT_TYPE>();
	mRecvFuncDic[(int)PACKET_TYPE::CONNECTION] = &LogicMain::ProcConnect;
	mRecvFuncDic[(int)PACKET_TYPE::DISCONNECTION] = &LogicMain::ProcDisConnect;
	mRecvFuncDic[(int)PACKET_TYPE::CS_LOGIN] = &LogicMain::ProcLogin;
	mRecvFuncDic[(int)PACKET_TYPE::CS_ROOM_LIST] = &LogicMain::ProcRoomList;
	mRecvFuncDic[(int)PACKET_TYPE::CS_ROOM_ENTER] = &LogicMain::ProcRoomEnter;
	mRecvFuncDic[(int)PACKET_TYPE::CS_ROOM_CHAT] = &LogicMain::ProcRoomChat;

	mClMgr = new ClientManager;
	mClMgr->Init(maxClient);

	//TODO const unsigned int 값 나중에 따로 옵션값으로 빼줄 것
	mMaxRoomCnt = 10;
	mRoomMgr = new RoomManager;
	mRoomMgr->Init(mMaxRoomCnt);
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

void LogicMain::RecvPktData(unsigned int unique_id, char* pBuf , int size)
{
	// OnRecv 에서 완성되서 온다. 
	std::lock_guard<std::mutex> guard(mLock);
		
	char *recvBuf = new char[size];
	memcpy(recvBuf, pBuf, size);

	auto pHeader = (PKT_HEADER*)pBuf;
	if (pHeader->packet_len < sizeof(PKT_HEADER))
	{
		return;
	}

	// 조립
	PacketFrame packet;
	packet.packet_type = pHeader->packet_type;
	packet.size = pHeader->packet_len;
	packet.pData = recvBuf;
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

		if (!mRecvPktQ.empty())
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

void LogicMain::ProcRecv(unsigned int uniqueId , const int pktType , unsigned int size , char* pData)
{
	auto iter = mRecvFuncDic.find(pktType);

	if (iter != mRecvFuncDic.end())
	{
		(this->*(iter->second))(uniqueId, size, pData);
	}
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
	std::lock_guard<std::mutex> guard(mLock);
	mConnectQueue.push(packet);
}



