#pragma once
#include "precompile.h"
// 락의 범위가 좁을 때 주로 사용한다.
// 
class FastSpinlock
{
public:
	FastSpinlock();
	~FastSpinlock();

	void EnterLock();
	void LeaveLock();

private:
	FastSpinlock(const FastSpinlock& rhs);
	FastSpinlock& operator=(const FastSpinlock& rhs);

	volatile long mLockFlag;
};

class FastSpinlockGuard
{
public:
	FastSpinlockGuard(FastSpinlock& lock) : mLock(lock)
	{
		mLock.EnterLock();
	}

	~FastSpinlockGuard()
	{
		mLock.LeaveLock();
	}

private:
	FastSpinlock& mLock;
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
	static FastSpinlock mLock;

	//friend struct LockGuard;
};

template <class TargetClass>
FastSpinlock ClassTypeLock<TargetClass>::mLock;