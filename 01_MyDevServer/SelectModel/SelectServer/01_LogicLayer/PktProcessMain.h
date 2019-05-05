#pragma once
#include <memory>

#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"
#include "../02_NetworkLayer/Define.h"


namespace NetworkLayer
{
	class SelectNetwork;
}

namespace LogicLayer
{
	class ClientManager;
	class LobbyManager;
	class Lobby;

	class PktProcessMain
	{
		using SelNet = NetworkLayer::SelectNetwork;
		using RecvPckInfo = PacketLayer::RecvPacketInfo;
		using PktID = PacketLayer::PACKET_ID;
		using NET_ERROR_SET = NetworkLayer::NET_ERROR_SET;


	public:
		PktProcessMain();
		~PktProcessMain();
		void Init(SelNet* pNetObj, ClientManager* pClientMgr, LobbyManager* pLobbyMgr);
		void Process(RecvPckInfo packet);
		void ProcLogin(RecvPckInfo packet);
		void ProcLobbyList(RecvPckInfo packet);
		void ProcLobbyEnter(RecvPckInfo packetInfo);


		void StateCheck();
	private:
		NetworkLayer::SelectNetwork* m_RefSelectNetObj;
		ClientManager* m_pRefClientMgr;
		LobbyManager* m_pRefLobbyMgr;

	};
}