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


	//TODO 옵션값으로 빼줄 것
	//TODO  굳이 힙에 할당 해줄 필요가 있을까 고민해보자
	mMaxRoomCnt = 10;
	mClMgr = std::make_unique<ClientManager>();
	mClMgr->Init(maxClient);

	mRoomMgr = std::make_unique<RoomManager>();
	mRoomMgr->Init(mMaxRoomCnt);
}

bool LogicMain::Start()
{
	mIsRun = true;
	mLogicThread = std::thread([this]() { LogicThread(); });
	return true;
}

void LogicMain::Stop()
{
	mIsRun = false;
	if (mLogicThread.joinable())
	{
		mLogicThread.join();
	}
}

void LogicMain::RecvPktData(unsigned int unique_id, char* pBuf , int size)
{
	// OnRecv 에서 완성되서 온다. 
		
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

	std::lock_guard<std::mutex> guard(mLock);
	mRecvPktQ.push(packet);
}

void LogicMain::ConnectionPktData(unsigned int unique_id, PACKET_TYPE type)
{
	PacketFrame packet;
	packet.packet_type = (unsigned short)type;
	packet.unique_id = unique_id;
	packet.size = 0;
	packet.pData = nullptr;

	std::lock_guard<std::mutex> guard(mLock);
	mRecvPktQ.push(packet);
}

void LogicMain::LogicThread()
{
	while (mIsRun)
	{
		if (!mRecvPktQ.empty())
		{
			std::lock_guard<std::mutex> guard(mLock);
			auto packet = mRecvPktQ.front();
			
			//delete [] mRecvPktQ.front().pData;
			mRecvPktQ.pop();
			
			if (packet.packet_type >= (int)PACKET_TYPE::CONNECTION)
			{
				ProcRecv(packet.unique_id, packet.packet_type, packet.size, packet.pData);
			}
			delete[] packet.pData;
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
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






