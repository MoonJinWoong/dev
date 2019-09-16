#pragma once
#include "preCompile.h"

class SessionManager;

class Session 
{
public:
	Session();
	virtual ~Session();
	Session(const Session& Other);
	Session(Session&& other);

	void SessionReset();

	bool PostAccept();
	bool AcceptCompletion();

	bool RealRecv();

	void Disconnect(int type);


	virtual void OnReceive(size_t len);
	virtual void OnDisconnect();
	virtual void OnRelease();

	void SendCompletion(DWORD transferred);
	void RecvCompletion(DWORD transferred);

	void AddRef();
	void ReleaseRef();


	bool IsConnected() const { return !!mConnected; }

public:
	static const int BUFSIZE = 8192;
private:

	SOCKADDR_IN		mClientAddr;
	//size_t mSendBufSize;
	//size_t mRecvBufSize;
	int				mSendPendingCount;
	volatile long	mRefCount;
	volatile long	mConnected;
	SOCKET mSocket;
	FastSpinLock	mSessionLock;


	// TODO. 원형 버퍼로 수정할 것
	char	mRecvBuffer[BUFSIZE];
	char	mSendBuffer[BUFSIZE];

	friend class ClientSessionManager;
};



