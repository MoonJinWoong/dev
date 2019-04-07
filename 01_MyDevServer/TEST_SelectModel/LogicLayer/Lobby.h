#pragma once

#include <vector>
#include <unordered_map>


namespace NetworkLib
{
	class NetworkFrame;
}

namespace LogicCommon
{
	enum class ERROR_CODE :short;
}
using ERROR_CODE = LogicCommon::ERROR_CODE;


namespace LogicLib
{
	using TcpNet = NetworkLib::NetworkFrame;

	class Player;
	class Room;

	struct LobbyPlayer
	{
		short Index = 0;
		Player* pPlayer = nullptr;
	};

	class Lobby
	{
	public:
		Lobby();
		~Lobby();

		void Init(const short lobbyIndex, const short maxLobbyUserCount
			, const short maxRoomCountByLobby, const short maxRoomUserCount);

		void SetNetwork(TcpNet* pNetwork);
		short GetIndex() { return m_LobbyIndex; }
		short GetUserCount();
		auto MaxUserCount() { return (short)m_MaxUserCount; }
		auto MaxRoomCount() { return (short)m_RoomList.size(); }
	private:

	protected:

		TcpNet* m_pRefNetwork;

		short m_LobbyIndex = 0;

		short m_MaxUserCount = 0;
		std::vector<LobbyPlayer> m_PlayerList;
		std::unordered_map<int, Player*> m_PlayerIndexDic;
		std::unordered_map<const char*, Player*> m_PlayerIDDic;

		std::vector<Room*> m_RoomList;
	};
}