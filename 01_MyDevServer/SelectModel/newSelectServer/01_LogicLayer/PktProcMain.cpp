#include "../02_NetworkLayer/SelectNetwork.h"
#include "Client.h"
#include "ClientManager.h"


#include "PktProcMain.h"

namespace LogicLayer
{
	PktProcMain::PktProcMain() {}
	PktProcMain::~PktProcMain() {}
	void PktProcMain::Init(SelectNet* NetObj , ClientManager* pClientMgr)
	{
		m_pSelecNetObj = NetObj;
		m_ClientMgr = pClientMgr;
	}
	void PktProcMain::Process(recvPacket packet)
	{
		auto packetId = packet.PacketId;

		switch (packetId)
		{
		case (int)PACKET_ID::CS_LOGIN:
		{
			ProcessLogin(packet);
			break;
		}
		case (int)PACKET_ID::CS_LOBBY_LIST:
		{
			ProcessLobbyList(packet);
			break;
		}

		}
	}
}