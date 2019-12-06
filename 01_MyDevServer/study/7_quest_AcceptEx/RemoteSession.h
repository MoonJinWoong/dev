#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <array>
const int MAX_SOCKBUF = 1024;	
const int MAX_WORKERTHREAD = 4; 

enum class IOOperation 
{
	RECV ,
	SEND,
	ACCEPT
};

//WSAOVERLAPPED����ü�� Ȯ��
struct CustomOverEx
{
	WSAOVERLAPPED mWSAOverlapped;		//Overlapped I/O����ü
	WSABUF		  mWSABuf;				//Overlapped I/O�۾� ����
	unsigned int  mUid = -1;			// unique id
	unsigned int  mRemainByte = 0;

	std::array<char, MAX_SOCKBUF> mBuf;
	int         mSendPos = 0;

	IOOperation mIoType;			//�۾� ���� ����

	CustomOverEx()
	{
		memset(&mWSAOverlapped, 0, sizeof(OVERLAPPED));
		memset(&mWSABuf, 0, sizeof(WSABUF));
		mBuf.fill(0);
	}
	void Init()
	{
		mBuf.fill(0);
	}
};

// ���� ����
class RemoteSession
{
public:

	RemoteSession();
	SOCKET& GetSock() { return mRemoteSock; }
	
	bool AcceptReady(SOCKET listenSock);
	bool AcceptFinish(HANDLE mIocp);

	bool SendReady(const unsigned int size, char* msg);
	bool SendPacket();
	void SendFinish(unsigned long len);

	bool RecvMsg();
	bool RecvReady();
	bool RecvFinish(const char* pNextBuf, const unsigned long remain);

	bool UnInit(bool IsForce, SOCKET mListenSock);
	
	void SetUniqueId(int& id) { mUID = id; }
	unsigned int GetUniqueId() const { return mUID; }
	bool IsLive() const { return mIsLive; }


	SOCKET						mRemoteSock;	
	CustomOverEx				mRecvOverEx;	
	CustomOverEx				mSendOverEx;	
	CustomOverEx                mAcceptOverEx;    
private:
	bool						mIsLive = false;

	std::mutex					mSendLock;
	int                         mSendBuffPos = 0;      // ���� ���� ��ġ
	int							mSendPendingCnt = 0;  // �� ������ ���� ��� üũ
	char						mSendReservedBuf[MAX_SOCKBUF]; //������ ����	
	char						mSendBuf[MAX_SOCKBUF]; //������ ����
	
	unsigned int				mUID = -1;
	char						mAcceptBuf[64];
};