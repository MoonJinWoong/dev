#pragma once
#include <thread>
#include <unordered_map>
#include <functional>
#include <queue>
#include <mutex>

#include "Packet.h"
#include "ClientManager.h"
#include "RoomManager.h"
class LogicMain
{
public:
	LogicMain() = default;
	~LogicMain() = default;

	void Init(unsigned int maxClient);
	bool Start();
	void Stop();

	void RecvPktData(unsigned int unique_id, char* msg , int size);

	void LogicThread();

	PacketFrame GetConnectPkt();
	void		PutConnectPkt(PacketFrame packet);

	//PacketFrame DeQueueRecvPkt();
	//void		EnQueueRecvPkt(unsigned int unique_id);

	void ProcRecv(unsigned int uniqueId, const int pktType, unsigned int size, char* pData);

	// 로직함수들이 이걸로 param 넣어서 태워 보낸다. 
	std::function<void(unsigned int, unsigned int, char*)> SendPacketFunc;

	// 로직처리 모음
	void ProcConnect(unsigned int uniqueId, int size, char* pData);
	void ProcDisConnect(unsigned int uniqueId, int size, char* pData);
	void ProcLogin(unsigned int uniqueId, int size, char* pData);
	void ProcRoomList(unsigned int uniqueId, int size, char* pData);
	void ProcRoomEnter(unsigned int uniqueId, int size, char* pData);
	void ProcRoomChat(unsigned int uniqueId, int size, char* pData);

private:
	bool isRun = false;
	std::thread mLogicThread;
	std::mutex mLock;
	std::queue<PacketFrame> mRecvPktQ;
	std::queue<PacketFrame> mConnectQueue;

	ClientManager* mClMgr;

	RoomManager* mRoomMgr;
	unsigned int mMaxRoomCnt = 0;
	typedef void(LogicMain::* RECV_PKT_TYPE)(unsigned int, int, char*);
	std::unordered_map<int, RECV_PKT_TYPE> mRecvFuncDic;
};

