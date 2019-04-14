#pragma once
#include <memory>


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
	private:
		std::unique_ptr<NetworkLayer::SelectNetwork> m_pNetworkObj;
		bool m_IsRun = false;
	protected:

	};
}