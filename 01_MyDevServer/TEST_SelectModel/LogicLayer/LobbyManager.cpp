
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

	void LobbyManager::SendLobbyListInfo(const int sessionIndex)
	{
		LogicCommon::PktLobbyListRes resPkt;
		resPkt.ErrorCode = (short)ERROR_SET::NONE;
		resPkt.LobbyCount = static_cast<short>(m_LobbyList.size());

		int index = 0;
		for (auto& lobby : m_LobbyList)
		{
			resPkt.LobbyList[index].LobbyId = lobby.GetIndex();
			resPkt.LobbyList[index].LobbyUserCount = lobby.GetUserCount();
			resPkt.LobbyList[index].LobbyMaxUserCount = lobby.MaxUserCount();

			++index;
		}

		// 보낼 데이터를 줄이기 위해 사용하지 않은 LobbyListInfo 크기는 빼고 보내도 된다.
		m_pRefNetwork->SendData(sessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(resPkt), (char*)&resPkt);
	}
}