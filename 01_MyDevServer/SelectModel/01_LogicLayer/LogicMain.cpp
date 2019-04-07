#include <iostream>
#include "LogicMain.h"

using NET_ERR_SET = NetworkLib::NET_ERR_SET;


namespace LogicLib
{
	LogicMain::LogicMain() {}
	LogicMain::~LogicMain() {}
	void LogicMain::Init()
	{
		m_pNetwork = std::make_unique<NetworkLib::SelectNetwork>();
		auto res = m_pNetwork->Init();
		if (res != NET_ERR_SET::NONE)
			std::cout << "ERROR ->" << (int)res << std::endl;

		m_IsRun = true;
	}
	void LogicMain::Run()
	{
		while (m_IsRun)
		{
			m_pNetwork->Run();

		}
	}
}