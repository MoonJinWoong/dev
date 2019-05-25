#include <iostream>
#include <thread>
#include "../LogicLayer/logicMain.h"

int main()
{
	LogicLayer::logicMain main;
	auto logicRet = main.Init();


	if (logicRet)
	{
		std::thread AcceptThread([&]() {
			main.RunAcceptThread(); }
		);

		std::thread logicThread([&]() {
			main.RunWorkerThread(); }
		);

		std::thread logicThread([&]() {
			main.RunLogicThread(); }
		);
		std::cout << "if you press any key , this app is stop...";
		getchar();

		main.StopLogicThread();
		logicThread.join();
	}

	std::cout << "logic init fail" << std::endl;



	return 0;
}