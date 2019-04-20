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
		m_pSelectNetwork = std::make_unique<NetworkLayer::SelectNetwork>();
		m_pSelectNetwork->InitNetwork();
		


		m_IsRun = true;
	}
	void LogicMain::Run()
	{
		while (m_IsRun)
		{
			m_pSelectNetwork->Run();
		}
	}

	void LogicMain::Stop()
	{
		m_IsRun = false;
	}


}