#include <iostream>
#include <thread>
#include "../01_LogicLayer/LogicMain.h"

int main()
{
	LogicLib::LogicMain main;
	main.Init();

	std::thread logicThread([&]() {
		main.Run(); }
	);

	logicThread.join();
}