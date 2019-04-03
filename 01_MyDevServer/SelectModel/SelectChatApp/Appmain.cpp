#pragma comment(lib,"Debug/LogicLayer.lib")

#include <iostream>
#include <thread>

#include "../LogicLayer/LogicMain.h"


int main()
{
	LogicLib::LogicMain main;
	main.Init();

	//std::thread logicThread([&]() {
	//	main.Run(); }
	//);

	//std::cout << "press any key to exit...";
	//getchar();

	//main.Stop();
	//logicThread.join();

	return 0;
}