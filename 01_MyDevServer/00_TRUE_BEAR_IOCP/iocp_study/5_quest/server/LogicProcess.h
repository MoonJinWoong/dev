#pragma once
#include "LogicMain.h"



void LogicMain::ProcConnect(u_Int clientIndex_, u_Short packetSize_, char* pPacket_)
{
	std::cout << "ProcConnect call" << std::endl;
}

void LogicMain::ProcDisConnect(u_Int clientIndex_, u_Short packetSize_, char* pPacket_)
{
	std::cout << "ProcDisConnect call" << std::endl;
}

void LogicMain::ProcLogin(u_Int clientIndex_, u_Short packetSize_, char* pPacket_)
{
	std::cout << "ProcLogin call" << std::endl;
}
