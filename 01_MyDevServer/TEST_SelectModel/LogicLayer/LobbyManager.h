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
		int MaxLobbyCount = 2;
		int MaxLobbyUserCount = 50;
		int MaxRoomCountByLobby = 20;
		int MaxRoomUserCount = 4;

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
		~LobbyManager();
		void Init(TcpNet* pNetwork);

		LobbyManagerConfig m_lobbyMgr;
		void SendLobbyListInfo(const int sessionIndex);
	private:
		
		TcpNet* m_pRefNetwork;

		std::vector<Lobby> m_LobbyList;

	protected:

	};
}