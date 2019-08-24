#include "Thread.h"

Thread::Thread(int threadCnt,HANDLE& hIocp):mThreadCnt(threadCnt) ,mhIocp(hIocp)
{
	cout << "[INFO] test : " << mhIocp << endl;
}
Thread::~Thread()
{

}

void Thread::CreateWorkerThread()
{
	vector<thread*> Worker_Threads;
	Worker_Threads.reserve(mThreadCnt);
	for (int i = 0; i < mThreadCnt; ++i)
	{
		Worker_Threads.push_back(new thread{ &Thread::RunWorker,this });
	}

	cout << "[INFO] Worker thread Create..." << endl;
	//  join 
	for (auto wor : Worker_Threads) { wor->join();  delete wor; }
	Worker_Threads.clear();
}


void Thread::RunWorker()
{
	while (true)
	{
		
	}
}