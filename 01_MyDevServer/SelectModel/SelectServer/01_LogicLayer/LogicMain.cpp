#include <thread>
#include <chrono>

#include "../02_NetworkLayer/ErrorSet.h"
#include "../02_NetworkLayer/Define.h"
#include "../02_NetworkLayer/SelectNetwork.h"

#include "LobbyManager.h"
#include "PktProcessMain.h"
#include "ClientManager.h"
#include "LogicMain.h"


namespace LogicLayer
{
	LogicMain::LogicMain() {}
	LogicMain::~LogicMain() {}
	void LogicMain::Init()
	{
		m_pSelectNetwork = std::make_unique<NetworkLayer::SelectNetwork>();
		m_pSelectNetwork->InitNetwork();
	

		m_pClientManager = std::make_unique<ClientManager>();
		m_pClientManager->Init();


		LobbyManagerOpt opt;


		m_pLobbyManager = std::make_unique<LobbyManager>();
		m_pLobbyManager->Init(m_pSelectNetwork.get(),opt);

		m_pPacketProcess = std::make_unique<PktProcessMain>();
		m_pPacketProcess->Init(m_pSelectNetwork.get(), m_pClientManager.get(),m_pLobbyManager.get());


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
					break;
				else
					// 로직 단에서 받은 패킷을 처리한다. 
					m_pPacketProcess->Process(packetInfo);
			}

			// 로직단에서 유저의 상태 체크 
			m_pPacketProcess->StateCheck();
		}
	}

	void LogicMain::Stop()
	{
		m_IsRun = false;
	}


}