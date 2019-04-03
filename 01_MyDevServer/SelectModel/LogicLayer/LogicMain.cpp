#pragma comment(lib,"Debug/NetworkLayer.lib")
#include <thread>
#include <chrono>

#include "../NetworkLayer/Defines.h"
#include "../NetworkLayer/SelectNetwork.h"
#include "LogicMain.h"



namespace LogicLib
{
	LogicMain::LogicMain() {}
	LogicMain::~LogicMain() {}
	void LogicMain::Init()
	{
		

		m_pNetwork = std::make_unique<NetworkLib::SelectNetwork>();
		auto result = m_pNetwork->Init();
		

	}
}

