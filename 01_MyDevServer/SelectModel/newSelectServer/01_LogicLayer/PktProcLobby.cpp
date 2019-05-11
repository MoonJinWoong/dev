#include "../02_NetworkLayer/ErrorSet.h"
#include "../02_NetworkLayer/SelectNetwork.h"
#include "Client.h"
#include "ClientManager.h"

#include "PktProcMain.h"


namespace LogicLayer
{
	void PktProcMain::ProcessLobbyList(recvPacket packet)
	{
		// 여기부터 구현해야함

		PacketLayer::SC_LobbyList_Pkt resPkt;

		resPkt.LobbyCount = 14;

		m_pSelecNetObj->SendInLogic(packet.clientIdx,
			(short)PACKET_ID::SC_LOBBY_LIST,
			sizeof(resPkt), (char*)& resPkt);
	}
}