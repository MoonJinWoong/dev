#pragma once

class Spinlock
{
public:
	Spinlock() : mLockFlag(0) {}
	~Spinlock() {}

	void Start()
	{
		for (int nloops = 0; ; nloops++)
		{
			if (InterlockedExchange(&mLockFlag, 1) == 0)
				return;

			Sleep((DWORD)min(10, nloops));
		}
	}
	void End()
	{
		InterlockedExchange(&mLockFlag, 0);
	}

private:
	Spinlock(const Spinlock& rhs);
	Spinlock& operator=(const Spinlock& rhs);

	volatile long mLockFlag;
};

class CustomSpinlockGuard
{
public:
	CustomSpinlockGuard(Spinlock& lock) : mLock(lock)
	{
		mLock.Start();
	}

	~CustomSpinlockGuard()
	{
		mLock.End();
	}

private:
	Spinlock& mLock;
};