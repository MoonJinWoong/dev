#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>

#include <thread>
#include <vector>

#include "RemoteClient.h"
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

	//���� ��û�� �����ϰ� �޼����� �޾Ƽ� ó���ϴ� �Լ�
	bool StartNetService(const unsigned int maxClientCount);

	//�����Ǿ��ִ� �����带 �ı��Ѵ�.
	void DestroyThread();


	void CreateClient(const unsigned int maxClientCount);

	//WaitingThread Queue���� ����� ��������� ����
	bool CreateWokerThread();

	//accept��û�� ó���ϴ� ������ ����
	bool CreateAccepterThread();

	//������� �ʴ� Ŭ���̾�Ʈ ���� ����ü�� ��ȯ�Ѵ�.
	RemoteClient* GetEmptyClientInfo();

	//CompletionPort��ü�� ���ϰ� CompletionKey��
	//�����Ű�� ������ �Ѵ�.
	bool BindIOCompletionPort(RemoteClient* pClientInfo);

	//WSARecv Overlapped I/O �۾��� ��Ų��.
	bool DoRecv(RemoteClient* pClientInfo);

	//WSASend Overlapped I/O�۾��� ��Ų��.
	bool DoSend(RemoteClient* pClientInfo, char* pMsg, int nLen);

	//Overlapped I/O�۾��� ���� �Ϸ� �뺸�� �޾� 
	//�׿� �ش��ϴ� ó���� �ϴ� �Լ�
	void WokerThread();

	//������� ������ �޴� ������
	void AccepterThread();

	//������ ������ ���� ��Ų��.
	void CloseSocket(RemoteClient* pClientInfo, bool bIsForce = false);


private:
	//IOCP ���� �Լ��� ���� ��ü
	Iocp mIocp;

	//Ŭ���̾�Ʈ ���� ���� ����ü
	std::vector<RemoteClient> mClientInfos;

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