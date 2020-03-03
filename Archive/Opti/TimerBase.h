#pragma once
#include <chrono>
using namespace std::chrono;
/*
	Ÿ�̸� ����� ����ϱ� ���� �ٺ� Ŭ����
*/

class TimerBase
{
public:
	TimerBase() :m_start(system_clock::time_point::min())
	{

	}

	void Clear() 
	{ 
		m_start = system_clock::time_point::min(); 
	}

	// �������̸� true
	bool IsStart() const
	{
		return (m_start.time_since_epoch() != system_clock::duration(0));
	}

	void Start()
	{
		m_start = system_clock::now();
	}

	// ����ð� ����
	unsigned long GetLap()
	{
		if(IsStart())
		{
			system_clock::duration diff;
			diff = system_clock::now() - m_start;
			return (unsigned)(duration_cast<milliseconds>(diff).count());
		}
		return 0;
	}
	

private:
	system_clock::time_point m_start;
};
