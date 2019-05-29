#pragma once
#include "preCompile.h"

#define MAX_IOCP_THREAD		SIZE_64

class IOCP
{
public:
	IOCP();
	~IOCP();



private:
	SOCKET					m_ListenSocket;
	HANDLE					m_IocpHandle;

	thread*					acceptThread;
	thread*					workerThread;

	bool					InitIOCP();
	void					AcceptThread();
	void					WorkerThread();
	bool					CreateThreads();

};
