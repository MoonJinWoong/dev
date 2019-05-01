
#include "Client.h"
#include "Room.h"
#include "Lobby.h"

namespace LogicLayer
{
	Lobby::Lobby() {}
	Lobby::~Lobby() {}
	void Lobby::Init(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount)
	{
		m_lobbyIndex = lobbyIndex;
		m_maxClientCnt = maxLobbyUserCount;

		for (int i = 0; i < m_maxClientCnt; ++i)
		{
			ClientInLobby lobbyClient;
			lobbyClient.Index = (short)i;
			lobbyClient.pUser = nullptr;

			m_ClientListInLobby.push_back(lobbyClient);
		}

		for (int i = 0; i < maxRoomCountByLobby; ++i)
		{
			m_RoomList.emplace_back(new Room());
			m_RoomList[i]->Init((short)i, maxRoomUserCount);
		}
	}
}