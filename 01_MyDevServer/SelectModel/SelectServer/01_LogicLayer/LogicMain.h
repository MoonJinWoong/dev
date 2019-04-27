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
		bool m_IsRun = false;
	protected:

	};
}