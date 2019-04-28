#pragma once
#include <memory>


#include "../02_NetworkLayer/Define.h"
//#include "../02_NetworkLayer/PacketInfo.h"
#include "../../PacketDefine/Packet.h"

namespace NetworkLayer
{
	class SelectNetwork;
}

namespace LogicLayer
{
	class ClientManager;


	class PktProcessMain
	{
		using SelNet = NetworkLayer::SelectNetwork;
		using RecvPckInfo = PacketLayer::RecvPacketInfo;
		using PktID = PacketLayer::PACKET_ID;


	public:
		PktProcessMain();
		~PktProcessMain();
		void Init(SelNet* pNetObj,ClientManager* pClientMgr);
		void Process(RecvPckInfo packet);
		void ProcLogin(RecvPckInfo packet);

		void StateCheck();
	private:
		NetworkLayer::SelectNetwork* m_RefSelectNetObj;
		ClientManager* m_pRefClientMgr;

	};
}