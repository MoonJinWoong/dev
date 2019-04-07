#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Player.h"

namespace NetworkLib { class NetworkFrame; }

namespace LogicCommon { enum class ERROR_CODE :short; }

using ERROR_CODE = LogicCommon::ERROR_CODE;

namespace LogicLib
{
	using TcpNet = NetworkLib::NetworkFrame;
	class Game;

	class Room
	{
	public:
		Room();
		~Room();
		void Init(const short index, const short maxUserCount);
		void SetNetwork(TcpNet* pNetwork);
	private:
		TcpNet* m_pRefNetwork;

		short m_Index = -1;
		short m_MaxUserCount;

		bool m_IsUsed = false;
		std::wstring m_Title;
		std::vector<Player*> m_UserList;

		Game* m_pGame = nullptr;
	protected:

	};
}