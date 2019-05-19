#pragma once

#include <vector>
#include <unordered_map>
//#include "Room.h"

namespace NetworkLayer
{
	class SelectNetwork;
}


enum class LOGIC_ERROR_SET :short;
using LOGIC_ERROR_SET = LOGIC_ERROR_SET;

namespace LogicLayer
{
	using SelectNet = NetworkLayer::SelectNetwork;

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
		virtual ~Lobby();

		void Init(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount);

		void Release();

		void SetNetwork(SelectNet* pNetwork);

		short GetIndex() { return m_LobbyIndex; }


		LOGIC_ERROR_SET Enter(Client* pUser);
		LOGIC_ERROR_SET LeaveUser(const int userIndex);

		short GetClientCnt();

		Room* CreateRoom();

		Room* GetRoom(const short roomIndex);

		auto MaxClientCount() { return (short)m_MaxUserCount; }

		auto MaxRoomCount() { return (short)m_RoomList.size(); }
		
		void BroadCastClient(const int clientIdx,const char* sendID,const char* msg  );


	protected:

	protected:
		Client* Find(const int Index);
		LOGIC_ERROR_SET Add(Client* pUser);
		void Remove(const int userIndex);


	protected:
		
		SelectNet* m_SelNetwork;

		short m_LobbyIndex = 0;

		short m_MaxUserCount = 0;
		std::vector<ClientInLobby> m_ClientList;
		std::unordered_map<int, Client*> m_SeachByIndex;
		std::unordered_map<const char*, Client*> m_SearchByStr;

		std::vector<Room*> m_RoomList;
	};
}

