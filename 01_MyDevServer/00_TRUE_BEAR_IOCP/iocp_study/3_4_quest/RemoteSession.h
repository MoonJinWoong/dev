#pragma once
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>

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

	bool SendMsg(const unsigned int size, char* msg);
	bool RecvMsg();

	void SetUniqueId(const unsigned int& id) { unique_id = id; }
	unsigned int GetUniqueId() const { return unique_id; }

private:
	SOCKET			mRemoteSock;			//Cliet�� ����Ǵ� ����
public:
	CustomOverEx	mRecvOver;	//RECV Overlapped I/O�۾��� ���� ����
	CustomOverEx	mSendOver;	//SEND Overlapped I/O�۾��� ���� ����
	unsigned int unique_id = -1;
};