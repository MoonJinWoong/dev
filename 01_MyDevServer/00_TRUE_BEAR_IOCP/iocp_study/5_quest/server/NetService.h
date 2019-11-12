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


	//������ �ʱ�ȭ�ϴ� �Լ�
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

	//������ ������ ���� ��Ų��.
	void CloseSocket(RemoteSession* pSession, bool bIsForce = false);

	bool SendMsg(c_u_Int unique_id, c_u_Int size, char* pData);

private:
	Iocp mIocp;
	std::vector<RemoteSession*> mVecSessions;
	SOCKET		mListenSocket = INVALID_SOCKET;
	int			mClientCnt = 0;
	
	std::vector<std::thread> mIOWorkerThreads;
	std::thread	mAccepterThread;

	//�۾� ������ ���� �÷���
	bool		mIsWorkerRun = true;
	bool		mIsAccepterRun = true;
	//���� ����
	char		mSocketBuf[1024] = { 0, };
};