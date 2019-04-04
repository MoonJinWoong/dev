#pragma once
#include <vector>
#include <unordered_map>

namespace NetworkLib
{
	class SelectNetwork;
}



namespace LogicLib
{
	struct LobbyManagerConfig
	{
		int MaxLobbyCount;
		int MaxLobbyUserCount;
		int MaxRoomCountByLobby;
		int MaxRoomUserCount;
	};

	struct LobbySmallInfo
	{
		short Num;
		short UserCount;
	};

	class Lobby;

	class LobbyManager
	{
		using TcpNet = NetworkLib::NetworkFrame;
		

	public:
		LobbyManager();
		virtual ~LobbyManager();

		void Init(const LobbyManagerConfig config, TcpNet* pNetwork);

		Lobby* GetLobby(short lobbyId);


	public:
		void SendLobbyListInfo(const int sessionIndex);





	private:
		TcpNet* m_pRefNetwork;
		std::vector<Lobby> m_LobbyList;

	};
}