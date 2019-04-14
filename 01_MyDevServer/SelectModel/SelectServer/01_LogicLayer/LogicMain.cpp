#include <thread>
#include <chrono>

#include "../02_NetworkLayer/SelectNetwork.h"

#include "LogicMain.h"


namespace LogicLayer
{
	LogicMain::LogicMain() {}
	LogicMain::~LogicMain() {}
	void LogicMain::Init()
	{
		m_pNetworkObj = std::make_unique<NetworkLayer::SelectNetwork>();
		auto result = m_pNetworkObj->Init();
		if (result == true)
			std::cout << "LogicLayer Init Success...." << std::endl;


		m_IsRun = true;
	}
	void LogicMain::Run()
	{
		while (m_IsRun)
		{
			m_pNetworkObj->Run();
		}
	}


}