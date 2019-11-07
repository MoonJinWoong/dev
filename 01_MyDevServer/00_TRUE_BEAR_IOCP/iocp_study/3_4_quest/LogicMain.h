#pragma once
#include <thread>
#include <mutex>
#include <unordered_map>
#include <queue>
#include "Packet.h"
#include "ClientManager.h"
//TODO: Ÿ�� ������ �ѱ����� ���Ƴֱ�
typedef const unsigned int c_u_Int;
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

private:
	bool isRun = false;
	std::thread mLogicThread;
	std::mutex mLock;
	std::queue<unsigned int> mUserIdQueue;
	std::queue<PacketFrame> mConnectQueue;

	ClientManager* mClMgr;


};