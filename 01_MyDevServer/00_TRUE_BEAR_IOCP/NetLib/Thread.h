#pragma once
#include "PreComplie.h"



class Thread
{
public:
	Thread(int threadCnt,HANDLE& mhIocp);
	~Thread();
	void CreateWorkerThread();
	void RunWorker();

	HANDLE mhIocp;
private:
		int mThreadCnt;

};