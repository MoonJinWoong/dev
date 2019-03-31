#pragma comment(lib,"x64/Debug/LogicLibrary")
#pragma comment(lib,"x64/Debug/NetworkLibrary")
#include "../LogicLibrary/LogicMain.h"



#include <iostream>
#include <thread>



int main()
{
	ChatLogic::LogicMain main;
	if (!main.Init()) std::cout << "Logic Init failed....!" << std::endl;


}

