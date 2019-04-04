#pragma once

#include <vector>
#include <unordered_map>

#include "Room.h"

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
	using TcpNet = NetworkLib::SelectNetwork;
	

	class Player;
	//class Room;

	struct LobbyUser
	{
		short Index = 0;
		Player* pUser = nullptr;
	};

	class Lobby
	{
	public:
		Lobby();
		virtual ~Lobby();

		void Init(const short lobbyIndex, const short maxLobbyUserCount, const short maxRoomCountByLobby, const short maxRoomUserCount);

		void Release();

		void SetNetwork(TcpNet* pNetwork);

		short GetIndex() { return m_LobbyIndex; }


		ERROR_CODE EnterUser(Player* pUser);
		ERROR_CODE LeaveUser(const int userIndex);

		short GetUserCount();


		Room* CreateRoom();

		Room* GetRoom(const short roomIndex);

		auto MaxUserCount() { return (short)m_MaxUserCount; }

		auto MaxRoomCount() { return (short)m_RoomList.size(); }


	protected:
		void SendToAllUser(const short packetId, const short dataSize, char* pData, const int passUserindex = -1);


	protected:
		Player* FindUser(const int userIndex);

		ERROR_CODE AddUser(Player* pUser);

		void RemoveUser(const int userIndex);


	protected:
		TcpNet* m_pRefNetwork;


		short m_LobbyIndex = 0;

		short m_MaxUserCount = 0;
		std::vector<LobbyUser> m_UserList;
		std::unordered_map<int, Player*> m_UserIndexDic;
		std::unordered_map<const char*, Player*> m_UserIDDic;

		std::vector<Room*> m_RoomList;
	};
}

