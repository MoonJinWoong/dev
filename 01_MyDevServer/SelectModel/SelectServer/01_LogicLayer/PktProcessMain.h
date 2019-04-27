#pragma once
#include <memory>


#include "../02_NetworkLayer/Define.h"
#include "../02_NetworkLayer/PacketInfo.h"
namespace NetworkLayer
{
	class SelectNetwork;
}

namespace LogicLayer
{
	class PktProcessMain
	{
		using SelNet = NetworkLayer::SelectNetwork;
		using RecvPckInfo = NetworkLayer::RecvPacketInfo;
		using PktID = NetworkLayer::PACKET_ID;
	public:
		PktProcessMain();
		~PktProcessMain();
		void Init(SelNet* pNetObj);
		void Process(RecvPckInfo packet);
		void ProcLogin(RecvPckInfo packet);

		void StateCheck();
	private:
		NetworkLayer::SelectNetwork* m_RefSelectNetObj;

	};
}