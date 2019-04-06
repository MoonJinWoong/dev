//#pragma comment(lib,"Debug/NetworkLayer.lib")
#include <thread>
#include <chrono>

#include "../NetworkLayer/Defines.h"
#include "../NetworkLayer/SelectNetwork.h"

#include "LobbyManager.h"
//
#include "PlayerManager.h"
#include "LogicMain.h"



namespace LogicLib
{
	LogicMain::LogicMain() {}
	LogicMain::~LogicMain() {}
	void LogicMain::Init()
	{
		m_pNetwork = std::make_unique<NetworkLib::SelectNetwork>();
		auto result = m_pNetwork->Init();
		
		m_pPlayerManager = std::make_unique<PlayerManager>();
		m_pPlayerManager->Init(2000);

		m_pLobbyManager = std::make_unique<LobbyManager>();
		m_pLobbyManager->Init(m_pNetwork.get());
	}
}

