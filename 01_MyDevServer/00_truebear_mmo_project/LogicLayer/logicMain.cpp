
#include "../NetworkLayer/Iocp.h"


#include "logicMain.h"


namespace LogicLayer
{
	logicMain::logicMain() {}
	logicMain::~logicMain() {}
	bool logicMain::Init()
	{
		m_IocpNet = std::make_unique<NetworkLayer::Iocp>();
		if (!m_IocpNet->InitNetwork()) return false;


		return true;
	}

	void logicMain::RunLogicThread()
	{

	}

	void logicMain::StopLogicThread()
	{

	}
}