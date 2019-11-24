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
	virtual void OnClose(c_u_Int index) {}
	virtual void OnRecv(c_u_Int index, c_u_Int size_, char* pData_) {}

	bool InitSocket();

	bool BindandListen(c_u_Int nBindPort);

	bool StartNetService(c_u_Int maxClientCount);

	void DestroyThread();

	bool CreateSessions(c_u_Int maxClientCount);

	bool CreateWokerThread();

	RemoteSession* GetEmptySession();
	RemoteSession* GetSessionByIdx(c_Int index) {return mVecSessions[index];}
	
	void DoSend(RemoteSession* pSessoin);

	void WokerThread();

	void KickOutSession(RemoteSession* pSession, bool isForce = false);

	bool SendMsg(c_u_Int unique_id, c_u_Int size, char* pData);

private:
	Iocp mIocp;
	std::vector<RemoteSession*> mVecSessions;
	SOCKET		mListenSocket = INVALID_SOCKET;
	int			mClientCnt = 0;
	
	std::vector<std::thread> mIOWorkerThreads;
	std::thread	mAccepterThread;

	bool		mIsWorkerRun = true;
	bool		mIsAccepterRun = true;
	
	//소켓 버퍼
	//char		mSocketBuf[1024] = { 0, };
};