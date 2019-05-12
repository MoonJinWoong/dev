#pragma once
#include <memory>
#include "../02_NetworkLayer/ErrorSet.h"
#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"


namespace NetworkLayer
{
	class SelectNetwork;
}

namespace LogicLayer
{
	using NET_ERROR_SET = NetworkLayer::NET_ERROR_SET;
	// 전방 선언
	class PktProcMain;
	class ClientManager;
	class LobbyManager;

	class LogicMain
	{
	public:
		LogicMain();
		~LogicMain();
		NET_ERROR_SET Init();
		void Run();
		void Stop();
	private:
		std::unique_ptr<NetworkLayer::SelectNetwork> m_SelectNetwork;

		std::unique_ptr<PktProcMain> m_PacketProc;
		std::unique_ptr<ClientManager> m_ClientMgr;
		std::unique_ptr<LobbyManager> m_LobbyMgr;
		bool m_IsRun = false;
	protected:

	};
}