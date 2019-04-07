#pragma once
#include <memory>

#include "Packet.h"
#include "ErrorSet.h"


using ERROR_SET = LogicCommon::ERROR_SET;

namespace NetworkLib
{
	class NetworkFrame;
}


namespace LogicLib
{
	class PlayerManager;
	class LobbyManager;
	class PacketProcess;

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
		std::unique_ptr<PacketProcess> m_pPacketProc;
	};
}