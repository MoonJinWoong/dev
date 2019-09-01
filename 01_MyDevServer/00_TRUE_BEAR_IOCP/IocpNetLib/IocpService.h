#pragma once
#include "preCompile.h"

// I/O Completion Port °´Ã¼.
class IocpThread;

class IocpService
{
public:
	IocpService();
	~IocpService();
	bool Init();

public:
	static LPFN_ACCEPTEX mFnAcceptEx;
	static LPFN_CONNECTEX mFnConnectEx;
	static char mAcceptBuf[64];

private:
	HANDLE	mCompletionPort;
	SOCKET	mListenSocket;

	IocpThread* mIoWorkerThread[MAX_IO_THREAD];

};

extern IocpService* gIocpService;




