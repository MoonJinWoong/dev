#pragma once

class FastSpinLock
{
public:
	FastSpinLock();
	~FastSpinLock();

	void EnterLock();
	void LeaveLock();

private:
	FastSpinLock(const FastSpinLock& rhs);
	FastSpinLock& operator=(const FastSpinLock& rhs);

	volatile long mLockFlag;
};

class FastSpinlockGuard
{
public:
	FastSpinlockGuard(FastSpinLock& lock) : mLock(lock)
	{
		mLock.EnterLock();
	}

	~FastSpinlockGuard()
	{
		mLock.LeaveLock();
	}

private:
	FastSpinLock& mLock;
};

template <class TargetClass>
class ClassTypeLock
{
public:
	struct LockGuard
	{
		LockGuard()
		{
			TargetClass::mLock.EnterLock();
		}

		~LockGuard()
		{
			TargetClass::mLock.LeaveLock();
		}

	};

private:
	static FastSpinLock mLock;

	//friend struct LockGuard;
};

template <class TargetClass>
FastSpinLock ClassTypeLock<TargetClass>::mLock;