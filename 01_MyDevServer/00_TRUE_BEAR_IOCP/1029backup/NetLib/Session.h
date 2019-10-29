
#pragma once
#include <mutex>
#include <atomic>
#include <string>

#define WIN32_LEAN_AND_MEAN 
#include <WinSock2.h>
#include <mswsock.h>
#include <Ws2tcpip.h>

#include "CircleBuffer.h"
#include "Defines.h"

class Session
{
public:
	//Session();
	Session(SOCKET listenSock, int index);
	~Session();

	SOCKET& GetRemoteSocket() { return mRemoteSock; }
	
	int GetIndex() { return mIndex; }
	
	CustomOverlapped* getRecvOverlappedEx() { return m_pRecvOverlappedEx; }
	CustomOverlapped* getSendOverlappedEx() { return m_pSendOverlappedEx; }


	//TODO 쓸거면 쓰고 안쓸거면 날려자. 코드 정리
	void IncrementRecvIORefCount() { InterlockedIncrement(reinterpret_cast<LPLONG>(&m_RecvIORefCount)); }
	//void IncrementSendIORefCount() { InterlockedIncrement(reinterpret_cast<LPLONG>(&m_SendIORefCount)); }
	//void IncrementAcceptIORefCount() { ++m_AcceptIORefCount; }
	void DecrementRecvIORefCount() { InterlockedDecrement(reinterpret_cast<LPLONG>(&m_RecvIORefCount)); }
	//void DecrementSendIORefCount() { InterlockedDecrement(reinterpret_cast<LPLONG>(&m_SendIORefCount)); }
	//void DecrementAcceptIORefCount() { --m_AcceptIORefCount; }

	bool SetNetStateDisConnection() { return InterlockedCompareExchange(reinterpret_cast<LPLONG>(&m_IsConnect), FALSE, TRUE); }
	
	bool CloseComplete();
	
	void DisconnectSession();

	bool GetLocalAndRemoteAddr();
	void SetRemoteIP(const char* szIP) { mIPAddr = szIP; }
	
	void  UpdateSessionState();
	
	void  AcceptOverlapped();
	
	bool PostRecv(const char* pNextBuf, const unsigned long remainByte);
	
	char* RecvBufferBeginPos() { return m_CircleRecvBuffer.GetBeginMark(); }
	
	int RecvBufferSize() { return m_CircleRecvBuffer.GetBufferSize(); }

private:
	CircleBuffer m_CircleRecvBuffer;
	CircleBuffer m_CircleSendBuffer;
	CustomOverlapped* m_pRecvOverlappedEx = nullptr;
	CustomOverlapped* m_pSendOverlappedEx = nullptr;

	int mIndex = -1;

	// 메모 : 초기화에서 리슨 소켓을 받아와서 바인딩하고
	// 통신을 할때는 사실상 mRemoteSock을 사용한다. 
	SOCKET mRemoteSock = INVALID_SOCKET;
	SOCKET mListenSock = INVALID_SOCKET;

	//TODO 스핀락가드로 갈 것
	// 질문 여기는 락을 거는 범위가 커질수도 있는데 스핀락으로 해야 하나요
	std::mutex m_MUTEX;


	char m_AddrBuf[MAX_ADDR_LENGTH] = { 0, };

	bool m_IsConnect = false;
	bool m_IsSendable = true;


	int	m_RecvBufSize = MAX_RECV_OVERLAPPED_BUFF_SIZE;
	int	m_SendBufSize = MAX_SEND_OVERLAPPED_BUFF_SIZE;

	std::string mIPAddr;

	unsigned long m_SendIORefCount = 0;
	unsigned long m_RecvIORefCount = 0;


	Message m_ConnectionMsg;
	Message m_CloseMsg;
	
};

