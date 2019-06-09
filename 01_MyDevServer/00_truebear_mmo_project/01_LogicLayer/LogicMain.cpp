#include "../02_NetworkLib/IOCP.h"
#include "LogicMain.h"


namespace LogicLayer
{
	LogicMain::LogicMain() {}
	LogicMain::~LogicMain() {}
	void LogicMain::Init()
	{
		m_iocpNet = std::make_unique<NetIOCP>();
		m_iocpNet->InitIocpServer();
		m_iocpNet->CreateThreads();
	}
	void LogicMain::Run()
	{

	}
	

}