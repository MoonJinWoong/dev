#include "PreComplie.h"
#include "Thread.h"


Thread::Thread(void)
{
	m_hThread = NULL;
	m_blsRun = false;
	m_dwWaitTick = 0;
	m_dwTickCount = 0;
	m_hQuitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

Thread::~Thread(void)
{
	CloseHandle(m_hQuitEvent);
	if (m_hThread)
		CloseHandle(m_hThread);
}


unsigned int WINAPI CallTickThread(LPVOID p)
{
	Thread* pTickThread = (Thread*) p;
	pTickThread->TickThread();

	return 1;
}

bool Thread::CreateThread(DWORD dwWaitTick)
{
	unsigned int uiThreadId = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &CallTickThread, this,
		CREATE_SUSPENDED, &uiThreadId);

	if (m_hThread == NULL)
	{
		LOG(LOG_ERROR_NORMAL, " SYSTEM | Thread::CreateTickThread() | TickThread 생성 실패 : Error(%u) "
			, GetLastError());
		return false;
	}
}