#pragma once
#include <thread>
#include <unordered_map>
#include <functional>
#include <queue>
#include "TypeDefine.h"
#include "Packet.h"
#include "ClientManager.h"

class LogicMain
{
public:
	LogicMain() = default;
	~LogicMain() = default;

	void Init(u_Int maxClient);

	bool Start();

	void Stop();

	void RecvPktData(u_Int unique_id, u_Int len, char* msg);

	void LogicThread();

	PacketFrame GetConnectPkt();
	void		PutConnectPkt(PacketFrame packet);

	PacketFrame GetUserPkt();
	void		PutUserIdx(u_Int unique_id);

	void ProcRecv(u_Int uniqueId, c_int pktType, u_Int size, char* pData);

	// 로직함수들이 이걸로 param 넣어서 태워 보낸다. 
	std::function<void(u_Int, u_Int, char*)> SendPacketFunc;


	// 로직처리 모음
	void ProcConnect(u_Int uniqueId, int size, char* pData);
	void ProcDisConnect(u_Int uniqueId, int size, char* pData);
	void ProcLogin(u_Int uniqueId, int size, char* pData);
	void ProcLobbyList(u_Int uniqueId, int size, char* pData);
private:
	bool isRun = false;
	std::thread mLogicThread;
	std::mutex mLock;
	std::queue<u_Int> mUserIdQueue;
	std::queue<PacketFrame> mConnectQueue;

	ClientManager* mClMgr;

	typedef void(LogicMain::* RECV_PKT_TYPE)(u_Int, int, char*);
	std::unordered_map<int, RECV_PKT_TYPE> mRecvFuncDic;
};

