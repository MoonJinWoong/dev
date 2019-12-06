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

	virtual void OnAccept(u_Int index) {}
	virtual void OnClose(u_Int index) {}
	virtual void OnRecv(u_Int index, u_Int size_, char* pData_) {}


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
	RemoteSession* GetSessionByIdx(c_Int index) {return mVecSessions[index];}
	
	
	//WSARecv Overlapped I/O
	bool DoRecv(RemoteSession* pSession);

	//WSASend Overlapped I/O
	void DoSend(RemoteSession* pSessoin);

	void WokerThread();
	void AccepterThread();

	void SendThread();
	bool SendQueuePush(u_Int size, char* pMsg);


	//소켓의 연결을 종료 시킨다.
	void CloseSocket(RemoteSession* pSession, bool bIsForce = false);

	bool SendMsg(u_Int unique_id, u_Int size, char* pData);

private:
	Iocp mIocp;
	std::vector<RemoteSession*> mVecSessions;
	SOCKET		mListenSocket = INVALID_SOCKET;
	int			mClientCnt = 0;
	
	std::vector<std::thread> mIOWorkerThreads;
	std::thread	mAccepterThread;
	std::thread	mSendThread;


	bool		mIsWorkerRun = true;
	bool		mIsAccepterRun = true;
	bool		mIsSendThreadRun = true;

	std::mutex					mSendLock;
	std::queue<u_Int>	mSendQ;

	//소켓 버퍼
	//char		mSocketBuf[1024] = { 0, };
};