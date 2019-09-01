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


	friend class ClientSessionManager;
};



