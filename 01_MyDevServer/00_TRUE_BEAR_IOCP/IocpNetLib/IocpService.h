#pragma once
#include "preCompile.h"

// I/O Completion Port °´Ã¼.
class Session;
class IocpThread;


class IocpService
{
public:
	IocpService();
	~IocpService();
	bool Init();
	bool CreateThreads();
	void RunAsyncAccept();
	void ShutDownService();

	SOCKET* GetListenSocket() { return &mListenSocket; }


public:
	static LPFN_ACCEPTEX mlpfnAcceptEx;
	static char mAcceptBuf[64];
private:
	HANDLE	mCompletionPort;
	SOCKET	mListenSocket;
	IocpThread* mIoWorkerThread[MAX_IO_THREAD];
	static unsigned int WINAPI IoWorkerThread(LPVOID lpParam);
};

extern IocpService* gIocpService;




