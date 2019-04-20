#include <iostream>
#include <thread>
#include "../01_LogicLayer/LogicMain.h"

int main()
{
	LogicLayer::LogicMain main;
	main.Init();

	std::thread logicThread([&]() {
		main.Run(); }
	);

	std::cout << "press any key to exit...";
	getchar();

	main.Stop();
	logicThread.join();


}