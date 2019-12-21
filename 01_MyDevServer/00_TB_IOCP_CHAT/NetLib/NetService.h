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

	virtual void PostLogicConnection(unsigned int unique_id,PACKET_TYPE type) = 0;
	virtual void PostLogicRecv(unsigned int unique_id, char* pData, unsigned int packetLen) = 0;

	bool InitSocket();
	bool BindandListen();
	bool StartNetService();

	bool CreateSessionPool();
	bool CreateWokerThread();
	bool CreateSendThread();
	void DestroyThread();

	RemoteSession* GetEmptySession();
	RemoteSession* GetSessionByIdx(int index) {return mSessionPool[index];}
	
	void OnRecv(CustomOverEx* pOver,unsigned long size);
	void OnSend(RemoteSession* pSessoin,unsigned long size);
	void OnAccept( unsigned int uid);

	void WokerThread();
	void SendThread();

	void OnCloseSession(RemoteSession* pSession, IO_TYPE ioType);
	void PostDisConnectProcess(RemoteSession* pSession);

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