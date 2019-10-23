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
	Session();
	~Session();
	void Init(SOCKET sock, int index);
	void AcceptOverlapped();

	int GetIndex() { return m_Index; }
	void IncrementRecvIORefCount() { InterlockedIncrement(reinterpret_cast<LPLONG>(&m_RecvIORefCount)); }
	//void IncrementSendIORefCount() { InterlockedIncrement(reinterpret_cast<LPLONG>(&m_SendIORefCount)); }
	//void IncrementAcceptIORefCount() { ++m_AcceptIORefCount; }
	void DecrementRecvIORefCount() { InterlockedDecrement(reinterpret_cast<LPLONG>(&m_RecvIORefCount)); }
	//void DecrementSendIORefCount() { InterlockedDecrement(reinterpret_cast<LPLONG>(&m_SendIORefCount)); }
	//void DecrementAcceptIORefCount() { --m_AcceptIORefCount; }

	bool SetNetStateDisConnection() { return InterlockedCompareExchange(reinterpret_cast<LPLONG>(&m_IsConnect), FALSE, TRUE); }

	int RecvBufferSize() { return m_RingRecvBuffer.GetBufferSize(); }

	bool SetNetAddressInfo();
	void SetRemoteIP(const char* szIP) { CopyMemory(m_szIP, szIP, MAX_IP_LENGTH); }
	bool CloseComplete();

	SOCKET GetClientSocket() { return m_ClientSocket; }
	bool BindIOCP(const HANDLE hWorkIOCP);
	void UpdateSessionState();

	bool PostRecv(const char* pNextBuf, const DWORD remainByte);
	char* RecvBufferBeginPos() { return m_RingRecvBuffer.GetBeginMark(); }

	void DisconnectSession();

public:
	CircleBuffer m_RingRecvBuffer;
	CircleBuffer m_RingSendBuffer;
	CustomOverlapped* m_pRecvOverlappedEx = nullptr;
	CustomOverlapped* m_pSendOverlappedEx = nullptr;
private:
	void InnerInit();
public:
	int m_Index = -1;

	SOCKET m_ClientSocket = INVALID_SOCKET;
	SOCKET m_ListenSocket = INVALID_SOCKET;

	std::mutex m_MUTEX;





	char m_AddrBuf[MAX_ADDR_LENGTH] = { 0, };

	BOOL m_IsClosed = FALSE;
	BOOL m_IsConnect = FALSE;
	BOOL m_IsSendable = TRUE;


	int	m_RecvBufSize = -1;
	int	m_SendBufSize = -1;

	char m_szIP[MAX_IP_LENGTH] = { 0, };

	DWORD m_SendIORefCount = 0;
	DWORD m_RecvIORefCount = 0;


	std::atomic<short> m_AcceptIORefCount = 0;

	Message m_ConnectionMsg;
	Message m_CloseMsg;
	
};

