#pragma once
#include <vector>
#include <unordered_map>

namespace NetworkLayer
{
	class SelectNetwork;
}

namespace LogicLayer
{

	struct LobbyManagerOpt
	{
		int MaxLobbyCnt = 2;
		int MaxLobbyClientCnt = 50;
		int MaxRoomCntByLobby = 20;
		int MaxRoomClientCount = 4;
	};
	class Lobby;
	using SelectNet = NetworkLayer::SelectNetwork;
	using NET_ERR_SET = NetworkLayer::NET_ERROR_SET;


	class LobbyManager
	{
	public:
		LobbyManager();
		~LobbyManager();
		void Init(SelectNet* network, const LobbyManagerOpt opt);
		NET_ERR_SET SendLobbyListInfo(const int Index);
		Lobby* GetLobby(short lobbyId);

	private:
		SelectNet* m_pSelectNet;
		std::vector<Lobby> m_LobbyList;
	};
}