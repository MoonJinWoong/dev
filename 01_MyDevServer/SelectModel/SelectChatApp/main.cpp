#include <iostream>

#include "../Logic/LogicMain.h"
#pragma comment(lib,"Debug/LogicLayer")
int main()
{
	LogicLib::LogicMain* logicObj = new LogicLib::LogicMain();
	
	if (logicObj->Init() == false) 
		std::cout << "Init Fail" << std::endl;


	

	return 0;
}

