#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"
#include "../02_NetworkLayer/SelectNetwork.h"
#include "Client.h"
#include "ClientManager.h"
#include "Lobby.h"
#include "LobbyManager.h"

#include "Room.h"
#include "PktProcMain.h"



namespace LogicLayer
{
	void PktProcMain::ProcessRoomEnter(recvPacket packet)
	{
		auto recvPacket = (PacketLayer::CS_Room_Enter*)packet.pRefData;
		PacketLayer::SC_Room_Enter sendPacket;

		auto client = m_ClientMgr->GetClient(packet.clientIdx);

		if (client == nullptr)
		{
			std::cout << "Get Client fail In ProcRoomEnter" << std::endl;
			return;
		}

		if (client->IsCurStateInLobby() == false)
		{
			std::cout << " this Client state is not lobby ...!!" << std::endl;
			return;
		}

		auto lobbyIdx = client->GetLobbyIndex();
		auto pLobby = m_LobbyMgr->GetLobby(lobbyIdx);
		if (pLobby == nullptr)
		{
			std::cout << "get lobby fail In LobbyMgr" << std::endl;
			return;
		}

		Room* room = nullptr;

		
		room = pLobby->GetRoom(recvPacket->roomIdBySelected);
		if (room == nullptr)
		{
			std::cout << "GetRoom is failed.." << std::endl;
			return;
		}

		auto enterRet = room->EnterClient(client);
		if (enterRet == false)
		{
			std::cout << "this romm is full....!" << std::endl;
			return;
		}
			


		client->EnterRoom(lobbyIdx, room->getIdx());

		room->BroadCastRoomEnter(client->GetClientIndex(), client->GetID().c_str());

		// TODO 방을 만드는것 구현
		
	}

}