#include "..//LogicLayer/LogicMain.h"



int main()
{
	// TODO : 옵션들 한군데로 몰아넣기
	// 로직 호출
	LogicMain logic_main;
	logic_main.Init();

	std::thread logicThread([&]() {
		logic_main.onStart(); }
	);


	std::cout << "if you want shutdown this Server..." << std::endl;
	std::cout << "command input -> "<<"show me the money"<< std::endl;

	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "show me the money")
		{
			std::cout << "shutdown..... !!!" << std::endl;
			break;
		}
	}


	logic_main.onShutDown();
	logicThread.join();

	return 0;
}