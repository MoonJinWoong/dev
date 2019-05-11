
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
	void LobbyManager::Init(SelectNet* network , const LobbyManagerOpt opt)
	{
		m_pSelectNet = network;


		for (int i = 0; i < opt.MaxLobbyCnt; ++i)
		{
			Lobby lobby;
			lobby.Init((short)i, (short)opt.MaxLobbyClientCnt,
				(short)opt.MaxRoomCntByLobby, (short)opt.MaxRoomClientCount);
			
			lobby.SetNetworkLobby(m_pSelectNet);

			m_LobbyList.push_back(lobby);
		}
	}

	NET_ERR_SET LobbyManager::SendLobbyListInfo(const int sessionIndex)
	{
		PacketLayer::PktLobbyListRes resPkt;

		resPkt.LobbyCount = static_cast<short>(m_LobbyList.size());

		int index = 0;
		for (auto& lobby : m_LobbyList)
		{
			std::cout << lobby.GetIndex() << "-" << lobby.GeClientCnt()
				<< "-" << lobby.MaxClientCnt() << std::endl;
			resPkt.LobbyList[index].LobbyId = lobby.GetIndex();
			resPkt.LobbyList[index].LobbyUserCount = lobby.GeClientCnt();
			resPkt.LobbyList[index].LobbyMaxUserCount = lobby.MaxClientCnt();
			++index;
		}

		// 보낼 데이터를 줄이기 위해 사용하지 않은 LobbyListInfo 크기는 빼고 보내도 된다.
		auto ret = m_pSelectNet->LogicSendBufferSet(sessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(resPkt), (char*)& resPkt);
		
		return ret;
	}

	Lobby* LobbyManager::GetLobby(short lobbyId)
	{
		if (lobbyId < 0 || lobbyId >= (short)m_LobbyList.size()) 
			return nullptr;
		

		return &m_LobbyList[lobbyId];
	}
}