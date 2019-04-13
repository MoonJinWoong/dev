#pragma once
#include <memory>
//
//#include "../../Common/Packet.h"
//#include "../../Common/ErrorCode.h"

//#include "../02_NetworkLayer/NetworkConfig.h"
#include "../02_NetworkLayer/SelectNetwork.h"

namespace LogicLib
{
	class LogicMain
	{
	public:
		LogicMain();
		~LogicMain();
		void Init();
		void Run();
	private:
		bool m_IsRun = false;
		std::unique_ptr<NetworkLib::SelectNetwork> m_pNetwork;
	protected:

	};
}