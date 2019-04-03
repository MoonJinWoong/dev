#pragma once
#include <memory>




namespace NetworkLib
{
	class NetworkFrame;
}


namespace LogicLib
{
	class LogicMain
	{
	public:
		LogicMain();
		~LogicMain();
		void Init();

	private:
		std::unique_ptr<NetworkLib::NetworkFrame> m_pNetwork;

	};
}