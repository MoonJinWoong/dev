#pragma once
#include <thread>
#include <chrono>


#include "PktProcessMain.h"


namespace NetworkLayer
{
	class SelectNetwork;
}

namespace LogicLayer
{
	// 전방 선언
	class ClientManager;
	class PktProcessMain;

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

		bool m_IsRun = false;
	protected:

	};
}