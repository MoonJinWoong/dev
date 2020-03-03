#pragma once
#pragma once
#include <vector>
#include <unordered_map>

namespace NetworkLayer
{
	class SelectNetwork;
}


namespace LogicLayer
{

		const int MaxLobbyCount = 10;
		const int MaxLobbyUserCount = 10;
		const int MaxRoomCountByLobby = 10;
		const int MaxRoomUserCount = 10;

	struct LobbySmallInfo
	{
		short Num;
		short UserCount;
	};

	class Lobby;

	class LobbyManager
	{
		using SelectNet = NetworkLayer::SelectNetwork;


	public:
		LobbyManager();
		virtual ~LobbyManager();

		void Init(SelectNet* pNetwork);

		Lobby* GetLobby(short lobbyId);


	public:
		void SendLobbyListInfo(const int sessionIndex);





	private:
		
		SelectNet* m_selectNet;

		std::vector<Lobby> m_LobbyList;

	};
}