#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <thread>
#include <vector>
#include <mutex>


#include "RemoteSession.h"
#include "Iocp.h"


class NetService
{
public:
	NetService() {}

	~NetService() { WSACleanup(); }

	virtual void ThrowLogicConnect(unsigned int index) = 0;
	virtual void ThrowLogicClose(unsigned int index) = 0;
	virtual void ThrowLogicRecv(CustomOverEx* pOver, unsigned int size_) = 0;


	//소켓을 초기화하는 함수
	bool InitSocket();

	bool BindandListen(unsigned int nBindPort);

	bool StartNetService(unsigned int maxClientCount);

	bool CreateSessionPool(unsigned int maxClientCount);
	bool CreateWokerThread();
	bool CreateSendThread();
	void DestroyThread();

	RemoteSession* GetEmptyClientInfo();
	RemoteSession* GetSessionByIdx(int index) {return mVecSessions[index];}
	
	
	void DoRecvFinish(CustomOverEx* pOver,unsigned long size);
	void DoSend(RemoteSession* pSessoin);
	void DoAcceptFinish(RemoteSession* pSession, unsigned int uid);


	void WokerThread();
	void SendThread();

	void KickSession(RemoteSession* pSession, bool bIsForce = false);

	bool SendMsg(unsigned int unique_id, unsigned int size, char* pData);

private:
	Iocp mIocpService;
	std::vector<RemoteSession*> mVecSessions;
	SOCKET		mListenSocket = INVALID_SOCKET;
	
	int			mSessionCnt = 0;
	const unsigned int mMaxSessionCnt = 100;   //TODO 나중에 옵션으로 따로 빼줄 것
	
	std::vector<std::thread> mIOWorkerThreads;
	std::thread	mAccepterThread;
	std::thread	mSendThread;


	bool		mWorkerRun = false;
	bool		mSendRun = false;

	std::mutex					mSendLock;
	std::queue<CustomOverEx*>	mSendQ;

};