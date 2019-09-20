#include "..//LogicLayer/LogicMain.h"



int main()
{
	// network 可记 持绢林磊

	// 肺流 龋免
	LogicMain logic_main;
	logic_main.Init();

	std::thread logicThread([&]() {
		logic_main.Start(); }
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


	logic_main.ShutDown();
	logicThread.join();

	return 0;
}