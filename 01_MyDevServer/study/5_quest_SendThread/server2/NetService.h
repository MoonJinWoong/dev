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

	virtual void OnAccept(unsigned int index) = 0;
	virtual void OnClose(unsigned int index) = 0;
	virtual void OnRecv(CustomOverEx* pOver, unsigned int size_) = 0;

	bool InitSocket();
	bool BindandListen(unsigned int nBindPort);
	bool StartNetService(unsigned int maxClientCount);

	void DestroyThread();

	bool CreateSessionPool(unsigned int maxClientCount);
	bool CreateWokerThread();
	bool CreateAccepterThread();
	bool CreateSendThread();

	RemoteSession* GetEmptyClientInfo();
	RemoteSession* GetSessionByIdx(int index) {return mVecSessions[index];}
	
	void DoRecv(RemoteSession* pSession);
	void DoSend(RemoteSession* pSessoin);

	void WokerThread();
	void SendThread();


	//소켓의 연결을 종료 시킨다.
	void KickOutSession(RemoteSession* pSession, bool isForce);

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


	bool		mIsWorkerRun = true;
	bool		mIsAccepterRun = true;
	bool		mIsSendThreadRun = true;

	std::mutex					mSendLock;
	std::queue<CustomOverEx*>	mSendQ;

};