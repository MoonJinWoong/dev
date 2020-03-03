#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <thread>
#include <vector>

#include "RemoteSession.h"
#include "Iocp.h"



class NetService
{
public:
	NetService() {}

	~NetService() { WSACleanup(); }

	virtual void OnAccept(const unsigned int index) {}
	virtual void OnClose(const unsigned int index) {}
	virtual void OnRecv(const unsigned int index, const unsigned int size_, char* pData_) {}


	//������ �ʱ�ȭ�ϴ� �Լ�
	bool InitSocket();



	//------������ �Լ�-------//
	//������ �ּ������� ���ϰ� �����Ű�� ���� ��û�� �ޱ� ���� 
	//������ ����ϴ� �Լ�
	bool BindandListen(int nBindPort);

	bool StartNetService(const unsigned int maxClientCount);

	void DestroyThread();

	void CreateClient(const unsigned int maxClientCount);

	bool CreateWokerThread();

	bool CreateAccepterThread();

	RemoteSession* GetEmptyClientInfo();

	//WSARecv Overlapped I/O �۾��� ��Ų��.
	bool DoRecv(RemoteSession* pSession);

	//WSASend Overlapped I/O�۾��� ��Ų��.
	bool DoSend(RemoteSession* pSession, char* pMsg, int nLen);

	//Overlapped I/O�۾��� ���� �Ϸ� �뺸�� �޾� 
	//�׿� �ش��ϴ� ó���� �ϴ� �Լ�
	void WokerThread();

	//������� ������ �޴� ������
	void AccepterThread();

	//������ ������ ���� ��Ų��.
	void CloseSocket(RemoteSession* pSession, bool bIsForce = false);


private:
	//IOCP ���� �Լ��� ���� ��ü
	Iocp mIocp;

	//���� ���� ���� ����ü
	std::vector<RemoteSession> mVecSessions;

	//Ŭ���̾�Ʈ�� ������ �ޱ����� ���� ����
	SOCKET		mListenSocket = INVALID_SOCKET;

	//���� �Ǿ��ִ� Ŭ���̾�Ʈ ��
	int			mClientCnt = 0;

	//IO Worker ������
	std::vector<std::thread> mIOWorkerThreads;

	//Accept ������
	std::thread	mAccepterThread;

	//CompletionPort��ü �ڵ�
	HANDLE		mIOCPHandle = INVALID_HANDLE_VALUE;

	//�۾� ������ ���� �÷���
	bool		mIsWorkerRun = true;

	//���� ������ ���� �÷���
	bool		mIsAccepterRun = true;
	//���� ����
	char		mSocketBuf[1024] = { 0, };
};