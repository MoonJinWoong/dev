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
	SEND,
	ACCEPT
};

//WSAOVERLAPPED����ü�� Ȯ��
struct CustomOverEx
{
	WSAOVERLAPPED	 m_wsaOverlapped;		
	WSABUF			 m_wsaBuf;				
	u_Int			 mUid = -1;			// unique id

	char		m_RecvBuf[MAX_SOCKBUF]; //������ ����
	char		m_SendBuf[MAX_SOCKBUF]; //������ ����
	IOOperation m_eOperation;			//�۾� ���� ����

};

// ���� ����
class RemoteSession
{
public:
	RemoteSession();

	bool AccpetAsync(SOCKET listenSock);
	bool AcceptFinish(HANDLE mhIocp);

	bool SendPushInLogic(c_u_Int size, char* pMsg);
	void SendPopInWorker();
	void SendMsg();

	bool RecvZeroByte();
	bool RecvStart();

	void SetUniqueId(c_u_Int& id) { mIdx = id; }
	u_Int GetUniqueId() const { return mIdx; }

	void SetConnected() { mIsConnected = true; }
	bool IsConnected() const { return mIsConnected; }

	void Release(bool IsForce,SOCKET mListenSock);

	SOCKET						mRemoteSock;			//Client�� ����Ǵ� ����
	SOCKET& GetSock() { return mRemoteSock; }

private:
	CustomOverEx				mOverIo;		// I/O�۾�
	CustomOverEx				mOverAccept;	// acceptEx �۾� 
	char						mAcceptBuf[64];
	
	std::mutex					mSendLock;
	std::queue<CustomOverEx*>	mSendQ;

	bool						mIsConnected = false;
	u_Int						mIdx = -1;
};