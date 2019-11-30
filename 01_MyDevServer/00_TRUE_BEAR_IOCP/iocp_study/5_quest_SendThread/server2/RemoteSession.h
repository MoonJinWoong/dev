#pragma once
#include "TypeDefine.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>
#include <queue>
#include <array>
const int MAX_SOCKBUF = 1024;	
const int MAX_WORKERTHREAD = 4; 

enum class IOOperation 
{
	RECV ,
	SEND
};

//WSAOVERLAPPED구조체를 확장
struct CustomOverEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O구조체
	WSABUF		  m_wsaBuf;				//Overlapped I/O작업 버퍼
	unsigned int  mUid = -1;			// unique id
	unsigned int  mRemainByte = 0;

	std::array<char, MAX_SOCKBUF> m_RecvBuf;
	std::array<char, MAX_SOCKBUF> m_SendBuf;
	int         mSendPos = 0;
	char* mCurrMark = nullptr;
	size_t      mCurrRecvPos = 0;
	size_t      mTotalRecvByte = 0;

	IOOperation m_eOperation;			//작업 동작 종류

	CustomOverEx()
	{
		memset(&m_wsaOverlapped, 0, sizeof(OVERLAPPED));
		memset(&m_wsaBuf, 0, sizeof(WSABUF));
		m_RecvBuf.fill(0);
		m_SendBuf.fill(0);
	}
	void Init()
	{
		m_RecvBuf.fill(0);
		m_SendBuf.fill(0);
		mTotalRecvByte = 0;
		mCurrRecvPos = 0;
	}
	bool IsMoreRecv(size_t size)
	{
		mCurrRecvPos += size;
		if (mCurrRecvPos < mTotalRecvByte)
			return true;
		else
			return false;
	}

	signed int CalTotalByte()
	{
		unsigned short PACKET_LEN = 2;
		unsigned short PACKET_TYPE = 2;
		short pos = 0;
		short packetLen[1] = { 0, };
		if (mTotalRecvByte == 0)
		{
			memcpy_s((void*)packetLen, sizeof(packetLen),
						(void*)m_RecvBuf.data(), sizeof(packetLen));
			mTotalRecvByte = (size_t)packetLen[0];
		}

		pos += sizeof(packetLen);
		return pos;
	}
};

// 원격 세션
class RemoteSession
{
public:

	RemoteSession();
	SOCKET& GetSock() { return mRemoteSock; }

	//bool SendPushInLogic(u_Int size, char* pMsg);
	//void SendPop();
	//void SendMsg();
	
	bool SendReady();
	bool SendPacket(char* pBuf , int len);
	void SendFinish(unsigned long len);

	bool RecvMsg();
	bool RecvReady();


	void SetUniqueId(int& id) { unique_id = id; }
	u_Int GetUniqueId() const { return unique_id; }
	int GetSendBuffPos() { return mSendBuffPos; }
	void IncreaseBuffPos(int len) { mSendBuffPos += len; }

	SOCKET						mRemoteSock;			//Cliet와 연결되는 소켓
	CustomOverEx				mOverEx;	//RECV Overlapped I/O작업을 위한 변수
	//CustomOverEx				mSendOver;	//SEND Overlapped I/O작업을 위한 변수
private:

	std::mutex					mSendLock;
	bool						mIsLive = false;
	//std::queue<CustomOverEx*>	mSendQ;
	//std::queue<RemoteSession*>	mSendQ;
	int                         mSendBuffPos = 0;      // 샌드 버퍼 위치
	int							mSendPendingCnt = 0;  // 다 보내지 못한 경우 체크
	unsigned int unique_id = -1;
};