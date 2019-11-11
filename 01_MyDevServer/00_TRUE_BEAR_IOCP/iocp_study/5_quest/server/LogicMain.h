#pragma once
#include <thread>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <queue>
#include "Packet.h"
#include "ClientManager.h"

//TODO: 타입 재정의 한군데에 몰아넣기
typedef const unsigned int c_u_Int;
typedef unsigned int u_Int;
typedef unsigned short u_Short;
typedef std::lock_guard<std::mutex> auto_lock;

class LogicMain
{
public:
	LogicMain() = default;
	~LogicMain() = default;

	void Init(c_u_Int maxClient);

	bool Start();

	void Stop();

	void RecvPktData(c_u_Int unique_id, c_u_Int len, char* msg);

	void LogicThread();

	PacketFrame GetConnectPkt();
	void		PutConnectPkt(PacketFrame packet);

	PacketFrame GetUserPkt();
	void		PutUserIdx(c_u_Int unique_id);

	void ProcRecv(PacketFrame& packet);

	// 로직함수들이 이걸로 param 넣어서 태워 보낸다. 
	std::function<void(u_Int, u_Int, char*)> SendPacketFunc;


	// 로직처리 모음
	void ProcConnect(u_Int uniqueId, u_Short size, char* pData);
	void ProcDisConnect(u_Int uniqueId, u_Short size, char* pData);
	void ProcLogin(u_Int uniqueId, u_Short size, char* pData);

private:
	bool isRun = false;
	std::thread mLogicThread;
	std::mutex mLock;
	std::queue<unsigned int> mUserIdQueue;
	std::queue<PacketFrame> mConnectQueue;

	ClientManager* mClMgr;

	typedef void(LogicMain::* RECV_PKT_TYPE)(unsigned int, unsigned short, char*);
	std::unordered_map<int, RECV_PKT_TYPE> mRecvFuncDic;
};