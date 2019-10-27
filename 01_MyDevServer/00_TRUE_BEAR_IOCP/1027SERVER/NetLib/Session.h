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


	//TODO �Լ� ���� ���� �����ؼ� �ٽ� ���� ��. ������ ������ �ٸ��� �����
	bool SetNetAddressInfo();
	void SetRemoteIP(const char* szIP) { mIPAddr = szIP; }
		//CopyMemory(m_szIP, szIP, MAX_IP_LENGTH); }
	void UpdateSessionState();
	void AcceptOverlapped();



	bool PostRecv(const char* pNextBuf, const DWORD remainByte);
	char* RecvBufferBeginPos() { return m_CircleRecvBuffer.GetBeginMark(); }
	int RecvBufferSize() { return m_CircleRecvBuffer.GetBufferSize(); }


	//TODO private�� �ٲ� ��
private:
	CircleBuffer m_CircleRecvBuffer;
	CircleBuffer m_CircleSendBuffer;
	CustomOverlapped* m_pRecvOverlappedEx = nullptr;
	CustomOverlapped* m_pSendOverlappedEx = nullptr;

	int mIndex = -1;

	// �޸� : �ʱ�ȭ���� ���� ������ �޾ƿͼ� ���ε��ϰ�
	// ����� �Ҷ��� mRemoteSock�� ����Ѵ�. 
	SOCKET mRemoteSock = INVALID_SOCKET;
	SOCKET mListenSock = INVALID_SOCKET;

	//TODO ���ɶ������ �� ��
	std::mutex m_MUTEX;


	char m_AddrBuf[MAX_ADDR_LENGTH] = { 0, };

	//TODO �Ʒ� �ΰ� ������ ���� ���� ��
	//TODO BOOL -> bool �� ����
	bool m_IsConnect;
	bool m_IsSendable;


	int	m_RecvBufSize = -1;
	int	m_SendBufSize = -1;

	//TODO ��Ʈ������ ����
	std::string mIPAddr;

	//TODO C++ Ÿ������ �ٽ� ����
	unsigned long m_SendIORefCount = 0;
	unsigned long m_RecvIORefCount = 0;

	//TODO �ϰ����ְ� �̰͵� interlocked�� ����
	std::atomic<short> m_AcceptIORefCount = 0;

	Message m_ConnectionMsg;
	Message m_CloseMsg;
	
};

