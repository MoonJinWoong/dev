
#include "../02_NetworkLayer/SelectNetwork.h"

#include "Client.h"
#include "ClientManager.h"
#include "Room.h"
#include "Lobby.h"
#include "LobbyManager.h"

#include "PktProcMain.h"

namespace LogicLayer
{
	PktProcMain::PktProcMain() {}
	PktProcMain::~PktProcMain() {}
	void PktProcMain::Init(SelectNet* NetObj , ClientManager* pClientMgr , LobbyManager* pLobbyMgr)
	{
		m_pSelecNetObj = NetObj;
		m_ClientMgr = pClientMgr;
		m_LobbyMgr = pLobbyMgr;
	}
	void PktProcMain::Process(recvPacket packet)
	{
		// TODO 
		// 이부분이 많아지면 오버헤드 
		// 개선을 해보자 
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
		case (int)PACKET_ID::CS_LOBBY_ENTER:
		{
			ProcessLobbyEnter(packet);
			break;
		}
		case (int)PACKET_ID::CS_LOBBY_CHAT:
		{
			ProcessLobbyChat(packet);
			break;
		}
		case (int)PACKET_ID::CS_ROOM_ENTER:
		{
			ProcessRoomEnter(packet);
			break;
		}

		}
	}
}