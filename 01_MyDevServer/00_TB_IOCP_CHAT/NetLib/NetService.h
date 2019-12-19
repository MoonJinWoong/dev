#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <thread>
#include <vector>
#include <mutex>

#include "Config.h"
#include "RemoteSession.h"
#include "Iocp.h"
#include "Packet.h"


class NetService
{
public:
	NetService() {}

	~NetService() { WSACleanup(); }

	virtual void ThrowLogicConnection(unsigned int index,PACKET_TYPE type) = 0;
	virtual void ThrowLogicRecv(CustomOverEx* pOver, unsigned int size) = 0;

	bool InitSocket();
	bool BindandListen();
	bool StartNetService();

	bool CreateSessionPool();
	bool CreateWokerThread();
	bool CreateSendThread();
	void DestroyThread();

	RemoteSession* GetEmptySession();
	RemoteSession* GetSessionByIdx(int index) {return mSessionPool[index];}
	
	void DoRecvFinish(CustomOverEx* pOver,unsigned long size);
	void DoSend(RemoteSession* pSessoin,unsigned long size);
	void DoAcceptFinish( unsigned int uid);

	void WokerThread();
	void SendThread();

	void KickSession(RemoteSession* pSession, IO_TYPE ioType);
	void ThrowDisConnectProcess(RemoteSession* pSession);

	bool SendMsg(unsigned int unique_id, unsigned int size, char* pData);
private:
	Iocp mIocpService;
	std::vector<RemoteSession*> mSessionPool;
	SOCKET		mListenSocket = INVALID_SOCKET;
	
	unsigned int mMaxSessionCnt;
	
	std::vector<std::thread> mIOWorkerThreads;
	std::thread	mAccepterThread;
	std::thread	mSendThread;

	bool		mWorkerRun = false;
	bool		mSendRun = false;

	std::mutex					mLock;
	std::queue<CustomOverEx*>	mSendQ;

};