#pragma once

#include <memory>

namespace NetworkLayer
{
	struct ServerConfig;
	
	class NetworkFrame;
}

namespace LogicLib
{
	class LogicMain
	{
	public:
		LogicMain();
		~LogicMain();

		bool Init();

		//void Run();

		//void Stop();


	private:
		std::unique_ptr<NetworkLayer::SelectNetwork> m_pNetwork;

	};
}

