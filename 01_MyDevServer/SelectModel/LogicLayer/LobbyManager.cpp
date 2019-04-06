
#include "../NetworkLayer/SelectNetwork.h"
#include "Packet.h"
#include "ErrorSet.h"

#include "Lobby.h"
#include "LobbyManager.h"


using ERROR_SET = LogicCommon::ERROR_SET;
using PACKET_ID = LogicCommon::PACKET_ID;

namespace LogicLib
{
	LobbyManager::LobbyManager() {}
	LobbyManager::~LobbyManager() {}
	void LobbyManager::Init(TcpNet* pNetwork)
	{
		m_pRefNetwork = pNetwork;

		for (int i = 0; i < m_lobbyMgr.MaxLobbyCount; ++i)
		{
			Lobby lobby;
			lobby.Init((short)i, (short)m_lobbyMgr.MaxLobbyUserCount
				, (short)m_lobbyMgr.MaxRoomCountByLobby
				, (short)m_lobbyMgr.MaxRoomUserCount);
			lobby.SetNetwork(m_pRefNetwork);
			m_LobbyList.push_back(lobby);
		}
	}
}