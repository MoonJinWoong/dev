#pragma once
#include <memory>




namespace NetworkLib
{
	class NetworkFrame;
}


namespace LogicLib
{
	class PlayerManager;
	class LobbyManager;

	class LogicMain
	{
	public:
		LogicMain();
		~LogicMain();
		void Init();

	private:
		std::unique_ptr<NetworkLib::NetworkFrame> m_pNetwork;
		std::unique_ptr<PlayerManager> m_pPlayerManager;
		std::unique_ptr<LobbyManager> m_pLobbyManager;
	};
}