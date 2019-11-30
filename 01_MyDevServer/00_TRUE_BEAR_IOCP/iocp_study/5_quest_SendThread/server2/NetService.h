#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <thread>
#include <vector>
#include "TypeDefine.h"
#include "RemoteSession.h"
#include "Iocp.h"


class NetService
{
public:
	NetService() {}

	~NetService() { WSACleanup(); }

	virtual void OnAccept(u_Int index) = 0;
	virtual void OnClose(u_Int index) = 0;
	virtual void OnRecv(CustomOverEx* pOver, u_Int size_) = 0;


	//소켓을 초기화하는 함수
	bool InitSocket();

	bool BindandListen(u_Int nBindPort);

	bool StartNetService(u_Int maxClientCount);

	void DestroyThread();

	void CreateClient(u_Int maxClientCount);
	bool CreateWokerThread();
	bool CreateAccepterThread();
	bool CreateSendThread();

	RemoteSession* GetEmptyClientInfo();
	RemoteSession* GetSessionByIdx(int index) {return mVecSessions[index];}
	
	
	//WSARecv Overlapped I/O
	void DoRecv(RemoteSession* pSession);

	//WSASend Overlapped I/O
	void DoSend(RemoteSession* pSessoin);

	void WokerThread();
	void AccepterThread();

	void SendThread();
	//bool SendQueuePush(u_Int size, char* pMsg);


	//소켓의 연결을 종료 시킨다.
	void CloseSocket(RemoteSession* pSession, bool bIsForce = false);

	bool SendMsg(u_Int unique_id, u_Int size, char* pData);

private:
	Iocp mIocp;
	std::vector<RemoteSession*> mVecSessions;
	SOCKET		mListenSocket = INVALID_SOCKET;
	
	int			mClientCnt = 0;
	const unsigned int mMaxSessionCnt = 3;   //TODO 나중에 옵션으로 따로 빼줄 것
	
	std::vector<std::thread> mIOWorkerThreads;
	std::thread	mAccepterThread;
	std::thread	mSendThread;


	bool		mIsWorkerRun = true;
	bool		mIsAccepterRun = true;
	bool		mIsSendThreadRun = true;

	std::mutex					mSendLock;
	//std::queue<RemoteSession*>	mSendQ;
	std::queue<CustomOverEx*>	mSendQ;

	//소켓 버퍼
	//char		mSocketBuf[1024] = { 0, };
};