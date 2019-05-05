#include "../02_NetworkLayer/SelectNetwork.h"

// #include "ConnectedClientManager
#include "Client.h"
#include "ClientManager.h"
#include "Room.h"
#include "Lobby.h"
#include "LobbyManager.h"
#include "PktProcessMain.h"



namespace LogicLayer
{
	PktProcessMain::PktProcessMain() {}
	PktProcessMain::~PktProcessMain() {}
	void PktProcessMain::Init(SelNet* pNetObj, ClientManager* pClientMgr , LobbyManager* pLobbyMgr)
	{
		// network layer select 객체 얻어와서 초기화 
		// client manager 전달 -> 이거 없으면 클라이언트 매니저 사용을 못함
		m_RefSelectNetObj = pNetObj;
		m_pRefClientMgr = pClientMgr;
		m_pRefLobbyMgr = pLobbyMgr;
		
	}
	void PktProcessMain::Process(RecvPckInfo packet)
	{
		auto packetID = packet.PacketId;

		switch (packetID)
		{
		case (int)PktID::LOGIN_IN_REQ:
		{
			this->ProcLogin(packet); break;
		}
			
		case (int)PktID::LOBBY_LIST_REQ:
		{
			this->ProcLobbyList(packet);
			break;
		}
		case (int)PktID::LOBBY_ENTER_REQ:
		{
			this->ProcLobbyEnter(packet);
			break;
		}
			
		default:
			break;

		}
	}
	void PktProcessMain::StateCheck() {}

}