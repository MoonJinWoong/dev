#pragma once

#include "preCompile.h"


enum IOType
{
	IO_NONE,
	IO_SEND,
	IO_RECV,
	IO_RECV_ZERO,
	IO_ACCEPT,
	IO_DISCONNECT
};

enum DisconnectType
{
	NONE,
	ACTIVE,
	ONCONNECT_ERROR,
	IO_REQUEST_ERROR,
	COMPLETION_ERROR,
	SENDFLUSH_ERROR,
	CONNECT_ERROR,
};



class Session;

struct ExOverlappedIO
{
	ExOverlappedIO(Session* owner, IOType ioType);

	OVERLAPPED	mOverlapped;
	Session* mSessionObject;
	IOType		mIoType;
	WSABUF		mWsaBuf;
	DisconnectType mDisConType;

};

void DeleteIoContext(ExOverlappedIO* context);