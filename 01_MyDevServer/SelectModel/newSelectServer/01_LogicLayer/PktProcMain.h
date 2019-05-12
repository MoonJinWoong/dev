#pragma once
#include <memory>

#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"
#include "../02_NetworkLayer/Define.h"


namespace NetworkLayer
{
	class SelectNetwork;
}

//using LOGIC_ERROR_SET = LogicLayer::LOGIC_ERROR_SET;


namespace LogicLayer
{
	using PACKET_ID = PacketLayer::PACKET_ID;
	using recvPacket = PacketLayer::RecvPacketInfo;

	using SelectNet = NetworkLayer::SelectNetwork;
	class ClientManager;
	class LobbyManager;


	class PktProcMain
	{

	public:
		PktProcMain();
		~PktProcMain();
		void Init(SelectNet* NetObj, ClientManager* pClientMgr, LobbyManager* pLobbyMgr);


		void Process(recvPacket packet);
		void ProcessLogin(recvPacket packet);
		void ProcessLobbyList(recvPacket packet);
		void ProcessLobbyEnter(recvPacket packet);
		void ProcessLobbyChat(recvPacket packet);

	private:
		SelectNet* m_pSelecNetObj;
		ClientManager* m_ClientMgr;
		LobbyManager* m_LobbyMgr;
	};
}