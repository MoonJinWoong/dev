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
		void Stop();
	private:
		std::unique_ptr<NetworkLayer::SelectNetwork> m_pSelectNetwork;
		bool m_IsRun = false;
	protected:

	};
}