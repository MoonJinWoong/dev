#pragma once
#include <vector>
#include <unordered_map>

namespace NetworkLayer
{
	class SelectNetwork;
}



namespace LogicLayer
{
	class Client;
	class Room;
	using SelectNet = NetworkLayer::SelectNetwork;
	struct ClientInLobby
	{
		short Index = 0;
		Client* pClient = nullptr;
	};

	class Lobby
	{
	public:
		Lobby();
		~Lobby();
		void Init(const short lobbyIndex, const short maxLobbyUserCount
			  , const short maxRoomCountByLobby, const short maxRoomUserCount);
		void  SetNetworkLobby(SelectNet* pNetwork);
		short GetIndex();
		short GeClientCnt();
		short  MaxClientCnt();
		short  MaxRoomCount();
		LOGIC_ERROR_SET Enter(Client* Client);
		Client* Find(const int userIndex);
		LOGIC_ERROR_SET Add(Client* pClient);


	protected:
		SelectNet* m_pSelectNet;

		short m_LobbyIndex;
		int m_maxClientCnt;

		std::vector<ClientInLobby> m_ClientListInLobby;
		std::unordered_map<int, Client*> m_ClientIndexSearch;
		std::unordered_map<const char*, Client*> m_ClientIDSearch;
		std::vector<Room*> m_RoomList;
	};
}