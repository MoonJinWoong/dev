#pragma once
#include <memory>

#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"


namespace NetworkLayer
{
	class SelectNetwork;
}

namespace LogicLayer
{
	// 전방 선언
	class ClientManager;
	class PktProcessMain;
	class LobbyManager;

	class LogicMain
	{
	public:
		LogicMain();
		~LogicMain();
		void Init();
		void Run();
		void Stop();
	private:
		std::unique_ptr<NetworkLayer::SelectNetwork> m_pSelectNetwork;
		std::unique_ptr<PktProcessMain> m_pPacketProcess;
		std::unique_ptr<ClientManager> m_pClientManager;
		std::unique_ptr<LobbyManager> m_pLobbyManager;
		bool m_IsRun = false;
	protected:

	};
}