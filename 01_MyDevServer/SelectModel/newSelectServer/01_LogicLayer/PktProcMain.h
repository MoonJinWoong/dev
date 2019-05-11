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
	using PACKET_ID = PacketLayer::PACKET_ID;
	using SelectNet = NetworkLayer::SelectNetwork;
	using recvPacket = PacketLayer::RecvPacketInfo;

	class ClientManager;

	class PktProcMain
	{

	public:
		PktProcMain();
		~PktProcMain();
		void Init(SelectNet* NetObj , ClientManager* pClientMgr);
		void Process(recvPacket packet);
		void ProcessLogin(recvPacket packet);
		void ProcessLobbyList(recvPacket packet);

	private:
		SelectNet* m_pSelecNetObj;

		ClientManager* m_ClientMgr;
	
	};
}