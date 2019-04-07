#pragma once
#include <time.h>
#include <chrono>
#include <vector>
#include "../NetworkLayer/SelectNetwork.h"


namespace LogicLib
{
	struct ConnectedPlayer
	{
		void Clear()
		{
			m_IsLoginSuccess = false;
			m_ConnectedTime = 0;
		}

		bool m_IsLoginSuccess = false;
		time_t m_ConnectedTime = 0;
	};

	class ConnectedPlayerManager
	{
		using TcpNet = NetworkLib::NetworkFrame;


	public:
		ConnectedPlayerManager() {}
		virtual ~ConnectedPlayerManager() {}

		void Init(const int maxSessionCount, TcpNet* pNetwork
			, NetworkLib::ServerConfig* pConfig)
		{
			m_pRefNetwork = pNetwork;

			for (int i = 0; i < maxSessionCount; ++i)
			{
				ConnectedUserList.emplace_back(ConnectedPlayer());
			}

			m_IsLoginCheck = pConfig->IsLoginCheck;
		}
		
		void SetLogin(const int sessionIndex)
		{
			ConnectedUserList[sessionIndex].m_IsLoginSuccess = true;
		}
	private:
		TcpNet* m_pRefNetwork;

		std::vector<ConnectedPlayer> ConnectedUserList;

		bool m_IsLoginCheck = false;

		std::chrono::system_clock::time_point m_LatestLoginCheckTime = std::chrono::system_clock::now();
		int m_LatestLogincheckIndex = -1;
	protected:

	};

}