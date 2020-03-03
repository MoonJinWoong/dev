#include <iostream>
#include "TimerBase.h"
# include <Windows.h>
#include <vector>


int main()
{
	TimerBase a;

	std::vector<int> aaa;
	aaa.reserve(50000000);

	a.Start();

	for (int i = 0; i < 50000000; ++i)
	{
		aaa.push_back(i);
	}

	std::cout << a.GetLap() << std::endl;


	return 0;
}