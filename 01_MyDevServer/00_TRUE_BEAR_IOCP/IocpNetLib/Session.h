#pragma once
#include "preCompile.h"

class SessionManager;
class Session 
{
public:
	Session();
	virtual ~Session();

	void SessionReset();

	bool PostAccept();
	bool AcceptCompletion();

	virtual void OnReceive(size_t len) {}
	virtual void OnDisconnect();
	virtual void OnRelease();

public:

private:

	SOCKADDR_IN		mClientAddr;
	size_t mSendBufSize;
	size_t mRecvBufSize;
	int				mSendPendingCount;
	volatile long	mRefCount;
	volatile long	mConnected;
	SOCKET mSocket;

	friend class ClientSessionManager;
};



