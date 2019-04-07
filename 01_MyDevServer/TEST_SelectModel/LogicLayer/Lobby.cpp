#include <algorithm>

#include "../NetworkLayer/SelectNetwork.h"
#include "Packet.h"
#include "ErrorSet.h"
#include "Player.h"
#include "Room.h"
#include "Lobby.h"

using PACKET_ID = LogicCommon::PACKET_ID;


namespace LogicLib
{
	Lobby::Lobby() {}
	Lobby::~Lobby() {}
	void Lobby::Init(const short lobbyIndex, const short maxLobbyUserCount
		, const short maxRoomCountByLobby, const short maxRoomUserCount)
	{
		m_LobbyIndex = lobbyIndex;
		m_MaxUserCount = (short)maxLobbyUserCount;

		for (int i = 0; i < maxLobbyUserCount; ++i)
		{
			LobbyPlayer lobbyUser;
			lobbyUser.Index = (short)i;
			lobbyUser.pPlayer = nullptr;

			m_PlayerList.push_back(lobbyUser);
		}

		for (int i = 0; i < maxRoomCountByLobby; ++i)
		{
			m_RoomList.emplace_back(new Room());
			m_RoomList[i]->Init((short)i, maxRoomUserCount);
		}
	}

	void Lobby::SetNetwork(TcpNet* pNetwork)
	{
		m_pRefNetwork = pNetwork;

		for (auto pRoom : m_RoomList)
		{
			pRoom->SetNetwork(pNetwork);
		}
	}
	short Lobby::GetUserCount()
	{
		return static_cast<short>(m_PlayerIndexDic.size());
	}
	
}