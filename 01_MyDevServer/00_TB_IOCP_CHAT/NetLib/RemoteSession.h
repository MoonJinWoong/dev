#pragma once
#include <winsock2.h>
#include <Windows.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>
#include <queue>
#include <mutex>
#include <array>
const int MAX_SOCKBUF = 1024;	

enum class IO_TYPE 
{
	RECV ,
	SEND,
	ACCEPT
};

struct CustomOverEx
{
	WSAOVERLAPPED mWSAOverlapped;		
	WSABUF		  mWSABuf;				
	unsigned int  mUid = -1;			// unique id
	unsigned int  mRemainByte = 0;
	std::array<char, MAX_SOCKBUF> mBuf;
	IO_TYPE mIoType;			

	CustomOverEx()
	{
		memset(&mWSAOverlapped, 0, sizeof(OVERLAPPED));
		memset(&mWSABuf, 0, sizeof(WSABUF));
		mBuf.fill(0);
	}
};

// 원격 세션
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
	bool RecvFinish(const char* pNextBuf, const unsigned long remain);

	bool UnInit(bool IsForce, SOCKET mListenSock);
	
	void SetUniqueId(int& id) { mUID = id; }
	unsigned int GetUniqueId() const { return mUID; }
	
	bool IsLive() const { return mIsLive; }
	void SetIsLive() const { InterlockedExchange((LPLONG)&mIsLive, true); }
	void UnSetIsLive() const { InterlockedExchange((LPLONG)&mIsLive, false); }

	SOCKET						mRemoteSock;	
	CustomOverEx				mRecvOverEx;	
	CustomOverEx				mSendOverEx;	
	CustomOverEx                mAcceptOverEx;  

private:
    bool						mIsLive = false;

	std::mutex					mSendLock;
	int                         mSendBuffPos = 0;    
	int							mSendPendingCnt = 0;  // 다 보내지 못한 경우 체크
	char						mSendReservedBuf[MAX_SOCKBUF]; 
	char						mSendBuf[MAX_SOCKBUF]; //데이터 버퍼
	
	unsigned int				mUID = -1;
	char						mAcceptBuf[64];
};