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
	SEND
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
	
	bool SendReady();
	bool SendPacket(char* pBuf , int len);
	void SendFinish(unsigned long len);

	bool RecvMsg();
	bool RecvReady();
	bool RecvFinish(const char* pNextBuf, const unsigned long remain);

	void SetUniqueId(int& id) { mUID = id; }
	unsigned int GetUniqueId() const { return mUID; }
	int GetSendBuffPos() { return mSendBuffPos; }
	void IncreaseBuffPos(int len) { mSendBuffPos += len; }

	SOCKET						mRemoteSock;			//Cliet�� ����Ǵ� ����
	CustomOverEx				mRecvOverEx;	//RECV Overlapped I/O�۾��� ���� ����
	CustomOverEx				mSendOverEx;	//SEND Overlapped I/O�۾��� ���� ����

private:
	std::mutex					mSendLock;
	bool						mIsLive = false;
	int                         mSendBuffPos = 0;      // ���� ���� ��ġ
	int							mSendPendingCnt = 0;  // �� ������ ���� ��� üũ
	unsigned int mUID = -1;
};