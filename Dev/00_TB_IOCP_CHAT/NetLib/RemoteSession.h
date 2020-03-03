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
#include "Config.h"
#include "CircleBuffer.h"

const int MAX_SOCKBUF = 1024;	

struct CustomOverEx
{
	WSAOVERLAPPED	mWSAOverlapped;		// Overlapped I/O����ü
	WSABUF			mWSABuf;			// Overlapped I/O�۾� ����
	int				mUid;				// unique id
	IO_TYPE			mIoType;			// �۾� ���� ����

	CustomOverEx()
	{
		memset(&mWSAOverlapped, 0, sizeof(OVERLAPPED));
		memset(&mWSABuf, 0, sizeof(WSABUF));
		mUid = -1;
	}
};

// ���� ����
class RemoteSession
{
public:
	RemoteSession();
	void Init();

	bool			AcceptReady(SOCKET listenSock);
	bool			AcceptFinish(HANDLE mIocp);

	bool			SendReady(const unsigned int size, char* msg);
	bool			SendIo();
	void			SendFinish(unsigned long len);

	bool			RecvIo(int remainSize);
	void			RecvFinish(unsigned short size);
	
	bool			CloseSocket();
	bool			DisconnectFinish(SOCKET mListenSock); //TODO �̸� �ٲٱ�

	void			SetUniqueId(int id) { mUID = id; }
	bool			IsLive()  { return mIsLive.load(); }

	unsigned int	GetUniqueId() const { return mUID; }
	SOCKET&			GetSock() { return mRemoteSock; }
	CircleBuffer&	GetRecvBuffer() { return mRecvBuffer; }
	CircleBuffer&	GetSendBuffer() { return mSendBuffer; }
	IoReference&	GetIoRef() { return mIoRef; }

private:
	SOCKET						mRemoteSock;

	CustomOverEx				mRecvOverEx;
	CustomOverEx				mSendOverEx;
	CustomOverEx                mAcceptOverEx;

	CircleBuffer				mSendBuffer;
	CircleBuffer				mRecvBuffer;
    
	std::atomic<bool>			mIsLive = false;
	IoReference					mIoRef;
	std::mutex					mLock;
	unsigned int				mUID = -1;
	char						mAcceptBuf[64];
};