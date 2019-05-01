#pragma once

#include <vector>
#include <unordered_map>


namespace LogicLayer
{
	class Client;
	class Room;


	struct ClientInLobby
	{
		short Index = 0;
		Client* pUser = nullptr;
	};

	class Lobby
	{
	public:
		Lobby();
		~Lobby();
		void Init(const short lobbyIndex, const short maxLobbyUserCount
			 , const short maxRoomCountByLobby, const short maxRoomUserCount);



	protected:
		short m_lobbyIndex;
		int m_maxClientCnt;

		std::vector<ClientInLobby> m_ClientListInLobby;
		std::vector<Room*> m_RoomList;
	};
}