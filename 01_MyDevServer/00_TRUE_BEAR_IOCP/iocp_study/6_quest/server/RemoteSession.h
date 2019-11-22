#pragma once
#include "TypeDefine.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>
#include <queue>

const int MAX_SOCKBUF = 1024;	//��Ŷ ũ��
const int MAX_WORKERTHREAD = 4;  //������ Ǯ�� ���� ������ ��

enum class IOOperation
{
	RECV,
	SEND
};

//WSAOVERLAPPED����ü�� Ȯ��
struct CustomOverEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O����ü
	SOCKET		m_socketClient;			//Ŭ���̾�Ʈ ����
	WSABUF		m_wsaBuf;				//Overlapped I/O�۾� ����

	char		m_RecvBuf[MAX_SOCKBUF]; //������ ����
	char		m_SendBuf[MAX_SOCKBUF]; //������ ����
	IOOperation m_eOperation;			//�۾� ���� ����

};

// ���� ����
class RemoteSession
{
public:
	RemoteSession();

	SOCKET& GetSock() { return mRemoteSock; }

	bool SendPushInLogic(c_u_Int size, char* pMsg);
	void SendPopInWorker();
	void SendMsg();

	bool RecvMsg();

	void SetUniqueId(c_u_Int& id) { unique_id = id; }
	u_Int GetUniqueId() const { return unique_id; }

	SOCKET						mRemoteSock;			//Cliet�� ����Ǵ� ����
private:

	CustomOverEx				mRecvOver;	//RECV Overlapped I/O�۾��� ���� ����
	//CustomOverEx				mSendOver;	//SEND Overlapped I/O�۾��� ���� ����
	
	std::mutex					mSendLock;
	std::queue<CustomOverEx*>	mSendQ;

	unsigned int unique_id = -1;
};