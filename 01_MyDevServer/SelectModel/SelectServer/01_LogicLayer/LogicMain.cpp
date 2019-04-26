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
			while (true)
			{
				auto packetInfo = m_pSelectNetwork->GetPacketInfo();

				if (packetInfo.PacketId == 0)
				{
					break;
				}
				else
				{
					// 로직단에서 받은 패킷을 처리한다. 
					m_pLogicPacketProcess->Process();
				}
			}

			// 로직단에서 유저의 상태 체크 
			m_pLogicPacketProcess->StateCheck();
		}
	}

	void LogicMain::Stop()
	{
		m_IsRun = false;
	}


}