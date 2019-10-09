#pragma once
#include <mutex>
#include <atomic>

#define WIN32_LEAN_AND_MEAN 
#include <WinSock2.h>
#include <mswsock.h>

#include "CircleBuffer.h"
#include "Defines.h"




class Session
{
public:
	Session(unsigned int UID);
	~Session();
	void AddCount();
	void Init();

	bool AcceptOverlapped(SOCKET& listenSocket, char* addr);
	void AcceptFinish(SOCKET& listenSocket,HANDLE &hIocp);
	
public:
	
	SOCKET			mSocket;
	SOCKADDR_IN		mClientAddr;
	LPFN_ACCEPTEX	mAcceptEx = NULL;


	CircleBuffer	mRecvBuffer;
	CircleBuffer	mSendBuffer;
	Spinlock		mSendBufferLock;
	int				mSendPendingCount;

	volatile long	mRefCount;
	volatile long	mConnected;
};


enum IOType
{
	IO_NONE,
	IO_SEND,
	IO_RECV,
	IO_RECV_ZERO,
	IO_ACCEPT,
	IO_DISCONNECT,
	IO_CONNECT
};


class CustomOverlapped
{
public:
	CustomOverlapped(Session* pSession, IOType ioType)
		: mSession(pSession), mIoType(ioType)
	{
		memset(&mOverlapped, 0, sizeof(WSAOVERLAPPED));
		memset(&mWsaBuf, 0, sizeof(WSABUF));
		mSession->AddCount();
	}

public:
	WSAOVERLAPPED	mOverlapped;
	Session* mSession;
	IOType		mIoType;
	WSABUF		mWsaBuf;

};