#pragma once

#include "Session.h"

class ClientSessionMgr;


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


};



