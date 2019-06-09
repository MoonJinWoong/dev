#include <iostream>
#include <thread>
#include "../01_LogicLayer/LogicMain.h"


int main()
{
	using LogicInstance = LogicLayer::LogicMain;

	LogicInstance *main= new LogicInstance();
	main->Init();


	std::thread logicThread([&]() {
		main->Run(); }
	);


	std::cout << "press any key to exit...";
	getchar();


	return 0;
}