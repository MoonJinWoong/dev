#pragma once

#include <Windows.h>
#include <iostream>

const int SPINLOCK_COUNT = 1000;

struct CustomSpinLockCriticalSection
{
	CRITICAL_SECTION m_CS;

	CustomSpinLockCriticalSection()
	{
		if (!InitializeCriticalSectionAndSpinCount(&m_CS, SPINLOCK_COUNT))
		{
			std::cout << "InitializeCriticalSectionAndSpinCount fail" << std::endl;
		}
	}

	~CustomSpinLockCriticalSection()
	{
		DeleteCriticalSection(&m_CS);
	}
};


class SpinLockGuard
{
public:
	explicit SpinLockGuard(CustomSpinLockCriticalSection* pCS)
		: m_pSpinCS(&pCS->m_CS)
	{
		EnterCriticalSection(m_pSpinCS);
	}

	~SpinLockGuard()
	{
		LeaveCriticalSection(m_pSpinCS);
	}

private:
	CRITICAL_SECTION* m_pSpinCS = nullptr;
};
