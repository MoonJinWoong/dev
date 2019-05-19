#include <thread>
#include <chrono>

#include "../02_NetworkLayer/ErrorSet.h"
#include "../02_NetworkLayer/Define.h"
#include "../02_NetworkLayer/SelectNetwork.h"

#include "LobbyManager.h"
#include "PktProcMain.h"
#include "ClientManager.h"
#include "LogicMain.h"


namespace LogicLayer
{
	LogicMain::LogicMain() {}
	LogicMain::~LogicMain() {}
	NET_ERROR_SET LogicMain::Init()
	{
		m_SelectNetwork = std::make_unique<NetworkLayer::SelectNetwork>();
		m_SelectNetwork->InitNetwork();


		m_ClientMgr = std::make_unique<ClientManager>();
		m_ClientMgr->Init(NetworkLayer::MAX_CLIENTS);

		m_LobbyMgr = std::make_unique<LobbyManager>();
		m_LobbyMgr->Init(m_SelectNetwork.get());

		m_PacketProc = std::make_unique<PktProcMain>();
		m_PacketProc->Init(m_SelectNetwork.get(),m_ClientMgr.get(),m_LobbyMgr.get());
	
		m_IsRun = true;
		return NET_ERROR_SET::NONE;
	}
	void LogicMain::Run()
	{
		while (m_IsRun)
		{
			m_SelectNetwork->Run();
			while (true)
			{
				auto packetInfo = m_SelectNetwork->GetPacketInfo();

				if (packetInfo.PacketId == 0)
					break;
				else
				{
					m_PacketProc->Process(packetInfo);
				}
					
				
			}

		}
	}

	void LogicMain::Stop()
	{
		m_IsRun = false;
	}


}