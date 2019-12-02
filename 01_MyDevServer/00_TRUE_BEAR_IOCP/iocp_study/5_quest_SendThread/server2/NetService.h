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


	//������ �ʱ�ȭ�ϴ� �Լ�
	bool InitSocket();

	bool BindandListen(unsigned int nBindPort);

	bool StartNetService(unsigned int maxClientCount);

	void DestroyThread();

	void CreateClient(unsigned int maxClientCount);
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
	//bool SendQueuePush(unsigned int size, char* pMsg);


	//������ ������ ���� ��Ų��.
	void CloseSocket(RemoteSession* pSession, bool bIsForce = false);

	bool SendMsg(unsigned int unique_id, unsigned int size, char* pData);

private:
	Iocp mIocpService;
	std::vector<RemoteSession*> mVecSessions;
	SOCKET		mListenSocket = INVALID_SOCKET;
	
	int			mSessionCnt = 0;
	const unsigned int mMaxSessionCnt = 100;   //TODO ���߿� �ɼ����� ���� ���� ��
	
	std::vector<std::thread> mIOWorkerThreads;
	std::thread	mAccepterThread;
	std::thread	mSendThread;


	bool		mIsWorkerRun = true;
	bool		mIsAccepterRun = true;
	bool		mIsSendThreadRun = true;

	std::mutex					mSendLock;
	std::queue<CustomOverEx*>	mSendQ;

};