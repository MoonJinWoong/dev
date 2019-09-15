#define _WINSOCKAPI_
#include <windows.h>
#include <Mmsystem.h>
#include "FastSpinlock.h"



FastSpinLock::FastSpinLock() : mLockFlag(0)
{
}


FastSpinLock::~FastSpinLock()
{
}


void FastSpinLock::EnterLock()
{
	for (int nloops = 0; ; nloops++)
	{
		if (InterlockedExchange(&mLockFlag, 1) == 0) {
			return;
		}

		UINT uTimerRes = 1;
		timeBeginPeriod(uTimerRes);
		Sleep((DWORD)min(10, nloops));
		timeEndPeriod(uTimerRes);
	}

}

void FastSpinLock::LeaveLock()
{
	InterlockedExchange(&mLockFlag, 0);
}