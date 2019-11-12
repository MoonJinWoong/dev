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


	//소켓을 초기화하는 함수
	bool InitSocket();

	bool BindandListen(c_u_Int nBindPort);

	bool StartNetService(c_u_Int maxClientCount);

	void DestroyThread();

	void CreateClient(c_u_Int maxClientCount);

	bool CreateWokerThread();

	bool CreateAccepterThread();

	RemoteSession* GetEmptyClientInfo();
	RemoteSession* GetSessionByIdx(c_Int index) {return mVecSessions[index];}
	
	
	//WSARecv Overlapped I/O
	bool DoRecv(RemoteSession* pSession);

	//WSASend Overlapped I/O
	void DoSend(RemoteSession* pSessoin, c_Int nLen);

	void WokerThread();
	void AccepterThread();

	//소켓의 연결을 종료 시킨다.
	void CloseSocket(RemoteSession* pSession, bool bIsForce = false);

	bool SendMsg(c_u_Int unique_id, c_u_Int size, char* pData);

private:
	Iocp mIocp;
	std::vector<RemoteSession*> mVecSessions;
	SOCKET		mListenSocket = INVALID_SOCKET;
	int			mClientCnt = 0;
	
	std::vector<std::thread> mIOWorkerThreads;
	std::thread	mAccepterThread;

	//작업 쓰레드 동작 플래그
	bool		mIsWorkerRun = true;
	bool		mIsAccepterRun = true;
	//소켓 버퍼
	char		mSocketBuf[1024] = { 0, };
};