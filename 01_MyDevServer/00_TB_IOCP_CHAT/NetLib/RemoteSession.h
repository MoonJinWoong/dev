#pragma once
#include <winsock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <array>
#include "IoDefine.h"
#include "CircleBuffer.h"

const int MAX_SOCKBUF = 1024;	

struct CustomOverEx
{
	WSAOVERLAPPED	mWSAOverlapped;		// Overlapped I/O구조체
	WSABUF			mWSABuf;			// Overlapped I/O작업 버퍼
	unsigned int	mUid;				// unique id
	IO_TYPE			mIoType;			// 작업 동작 종류

	CustomOverEx()
	{
		memset(&mWSAOverlapped, 0, sizeof(OVERLAPPED));
		memset(&mWSABuf, 0, sizeof(WSABUF));
		mUid = -1;
	}
};

// 원격 세션
class RemoteSession
{
public:
	RemoteSession();
	
	bool			AcceptReady(SOCKET listenSock);
	bool			AcceptFinish(HANDLE mIocp);

	bool			SendReady(const unsigned int size, char* msg);
	bool			SendPacket();
	void			SendFinish(unsigned long len);

	bool			RecvMsg();
	void			RecvFinish(unsigned short size);
	
	void			SetUniqueId(int& id) { mUID = id; }
	bool			IsLive()  { return mIsLive.load(); }
	void			SetIsLive() { mIsLive.store(true); }

	void			UnSetIsLive()  { mIsLive.store(false); }
	bool			UnInit(bool IsForce, SOCKET mListenSock);

	unsigned int	GetUniqueId() const { return mUID; }
	SOCKET&			GetSock() { return mRemoteSock; }
	CircleBuffer&	GetRecvBuffer() { return mRecvBuffer; }
	CircleBuffer&	GetSendBuffer() { return mSendBuffer; }

private:
	SOCKET						mRemoteSock;

	CustomOverEx				mRecvOverEx;
	CustomOverEx				mSendOverEx;
	CustomOverEx                mAcceptOverEx;

	CircleBuffer				mSendBuffer;
	CircleBuffer				mRecvBuffer;
    std::atomic<bool>			mIsLive = false;
	
	unsigned int				mUID = -1;
	char						mAcceptBuf[64];
};