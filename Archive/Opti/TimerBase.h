#pragma once
#include <chrono>
using namespace std::chrono;
/*
	타이머 기능을 사용하기 위한 근본 클래스
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

	// 실행중이면 true
	bool IsStart() const
	{
		return (m_start.time_since_epoch() != system_clock::duration(0));
	}

	void Start()
	{
		m_start = system_clock::now();
	}

	// 경과시간 얻어옴
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
