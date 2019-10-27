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
	Session();
	Session(SOCKET listenSock, int index);
	~Session();

	SOCKET& GetRemoteSocket() { return mRemoteSock; }
	int GetIndex() { return mIndex; }
	CustomOverlapped* getRecvOverlappedEx() { return m_pRecvOverlappedEx; }
	CustomOverlapped* getSendOverlappedEx() { return m_pSendOverlappedEx; }



	void IncrementRecvIORefCount() { InterlockedIncrement(reinterpret_cast<LPLONG>(&m_RecvIORefCount)); }
	//void IncrementSendIORefCount() { InterlockedIncrement(reinterpret_cast<LPLONG>(&m_SendIORefCount)); }
	//void IncrementAcceptIORefCount() { ++m_AcceptIORefCount; }
	void DecrementRecvIORefCount() { InterlockedDecrement(reinterpret_cast<LPLONG>(&m_RecvIORefCount)); }
	//void DecrementSendIORefCount() { InterlockedDecrement(reinterpret_cast<LPLONG>(&m_SendIORefCount)); }
	//void DecrementAcceptIORefCount() { --m_AcceptIORefCount; }

	bool SetNetStateDisConnection() { return InterlockedCompareExchange(reinterpret_cast<LPLONG>(&m_IsConnect), FALSE, TRUE); }
	bool CloseComplete();
	void DisconnectSession();


	//TODO 함수 엔터 간격 생각해서 다시 맞출 것. 같으면 모으고 다르면 띄워서
	bool SetNetAddressInfo();
	void SetRemoteIP(const char* szIP) { mIPAddr = szIP; }
		//CopyMemory(m_szIP, szIP, MAX_IP_LENGTH); }
	void UpdateSessionState();
	void AcceptOverlapped();



	bool PostRecv(const char* pNextBuf, const DWORD remainByte);
	char* RecvBufferBeginPos() { return m_CircleRecvBuffer.GetBeginMark(); }
	int RecvBufferSize() { return m_CircleRecvBuffer.GetBufferSize(); }


	//TODO private로 바꿀 것
private:
	CircleBuffer m_CircleRecvBuffer;
	CircleBuffer m_CircleSendBuffer;
	CustomOverlapped* m_pRecvOverlappedEx = nullptr;
	CustomOverlapped* m_pSendOverlappedEx = nullptr;

	int mIndex = -1;

	// 메모 : 초기화에서 리슨 소켓을 받아와서 바인딩하고
	// 통신을 할때는 mRemoteSock을 사용한다. 
	SOCKET mRemoteSock = INVALID_SOCKET;
	SOCKET mListenSock = INVALID_SOCKET;

	//TODO 스핀락가드로 갈 것
	std::mutex m_MUTEX;


	char m_AddrBuf[MAX_ADDR_LENGTH] = { 0, };

	//TODO 아래 두개 변수는 같이 가도 됨
	//TODO BOOL -> bool 로 변경
	bool m_IsConnect;
	bool m_IsSendable;


	int	m_RecvBufSize = -1;
	int	m_SendBufSize = -1;

	//TODO 스트링으로 변경
	std::string mIPAddr;

	//TODO C++ 타입으로 다시 변경
	unsigned long m_SendIORefCount = 0;
	unsigned long m_RecvIORefCount = 0;

	//TODO 일관성있게 이것도 interlocked로 갈것
	std::atomic<short> m_AcceptIORefCount = 0;

	Message m_ConnectionMsg;
	Message m_CloseMsg;
	
};

