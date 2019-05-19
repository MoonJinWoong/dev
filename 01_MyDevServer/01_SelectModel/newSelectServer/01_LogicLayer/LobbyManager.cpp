#include "../02_NetworkLayer/SelectNetwork.h"
#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"

#include "Lobby.h"
#include "LobbyManager.h"

using PACKET_ID = PacketLayer::PACKET_ID;

namespace LogicLayer
{
	LobbyManager::LobbyManager() {}

	LobbyManager::~LobbyManager() {}


	void LobbyManager::Init(SelectNet* pNetwork)
	{
		m_selectNet = pNetwork;

		for (int i = 0; i < MaxLobbyCount; ++i)
		{
			Lobby lobby;
			lobby.Init((short)i, MaxLobbyUserCount, MaxRoomCountByLobby, (short)MaxRoomUserCount);
			lobby.SetNetwork(m_selectNet);
			m_LobbyList.push_back(lobby);
		}
	}

	Lobby* LobbyManager::GetLobby(short lobbyId)
	{
		if (lobbyId < 0 || lobbyId >= (short)m_LobbyList.size()) {
			return nullptr;
		}

		return &m_LobbyList[lobbyId];
	}

	void LobbyManager::SendLobbyListInfo(int Index)
	{
		PacketLayer::SC_LobbyList_Pkt packet;
		
		packet.LobbyCount = (short)m_LobbyList.size();

		int index = 0;
		for (auto& lobby : m_LobbyList)
		{
			packet.LobbyList[index].LobbyID = lobby.GetIndex();
			packet.LobbyList[index].LobbyClientCount = lobby.GetClientCnt();
			packet.LobbyList[index].LobbyMaxClientCount = lobby.MaxClientCount();

			++index;
		}

		// 보낼 데이터를 줄이기 위해 사용하지 않은 LobbyListInfo 크기는 빼고 보내도 된다.
		m_selectNet->SendInLogic(Index, (short)PACKET_ID::SC_LOBBY_LIST
			, sizeof(packet), (char*)& packet);
	}

}