#pragma once
#include <thread>
#include <chrono>

#include "LogicProcess.h"


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
		std::unique_ptr<LogicProcess> m_pLogicPacketProcess;
		bool m_IsRun = false;
	protected:

	};
}