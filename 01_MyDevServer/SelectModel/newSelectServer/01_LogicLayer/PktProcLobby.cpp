#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"
#include "../02_NetworkLayer/SelectNetwork.h"
#include "Client.h"
#include "ClientManager.h"
#include "Lobby.h"
#include "LobbyManager.h"
#include "PktProcMain.h"

#include <tuple>
namespace LogicLayer
{
	void PktProcMain::ProcessLobbyList(recvPacket packet)
	{
		// 유저 매니저에서 유저를 가져오고 
		auto client = m_ClientMgr->GetClient(packet.clientIdx);
		
		if (client == nullptr)
		{
			std::cout << "Get Client in Lobby List is fail" << std::endl;
			return;
		}

		auto client_data = client;
		if(client_data->IsCurStateInLogIn() == false)
			std::cout << "this client state is not Lobby..!! " << std::endl;
		
		// 로비 매니저에서 send 해준다.
		m_LobbyMgr->SendLobbyListInfo(packet.clientIdx);

		// TODO 아래에 예외 처리 해주어야 할 듯

	}

	void PktProcMain::ProcessLobbyEnter(recvPacket packet)
	{

		auto recvPacket = (PacketLayer::CS_LobbyEnter_Pkt*)packet.pRefData;
		PacketLayer::SC_LobbyEnter_Pkt sendPacket;

		auto client = m_ClientMgr->GetClient(packet.clientIdx);


		if (client == nullptr)
		{
			std::cout << "Get Client in Lobby Enter is fail" << std::endl;
			return;
		}

		if (client->IsCurStateInLogIn() == false) 
		{
			std::cout << "this Client state is not lobby" << std::endl;
			return;
		}

		auto pLobby = m_LobbyMgr->GetLobby(recvPacket->lobbyIdBySelected);
		if (pLobby == nullptr) 
		{
			// 유저에게 보내줘야함
			return;
		}
		
		auto enterRet = pLobby->Enter(client);
		if (enterRet != LOGIC_ERROR_SET::NONE) 
		{
			// 유저에게 보내줘야함
			return;
		}
		sendPacket.MaxUserCount = pLobby->MaxClientCount();
		sendPacket.MaxRoomCount = pLobby->MaxRoomCount();
		
		strcpy(sendPacket.msg, "lobby enter success");

			

		m_pSelecNetObj->SendInLogic(packet.clientIdx,
			(short)PACKET_ID::SC_LOBBY_ENTER, sizeof(PacketLayer::SC_LobbyEnter_Pkt),
			(char*)& sendPacket);
	}

	void PktProcMain::ProcessLobbyChat(recvPacket packet)
	{
		auto recvPacket = (PacketLayer::CS_Lobby_Chat_Pkt*)packet.pRefData;
		PacketLayer::SC_Lobby_Chat_Pkt sendPacket;

		auto client = m_ClientMgr->GetClient(packet.clientIdx);


		// error
		if (client == nullptr || client->IsCurStateInLobby() == false)
		{
			std::cout << "Get Client in Lobby Chat is fail" << std::endl;

			PacketLayer::SC_Error_Msg errResponse;
			strcpy(errResponse.msg, "you need join lobby ...!");
			m_pSelecNetObj->SendInLogic(packet.clientIdx, (short)PACKET_ID::SC_ERROR_MSG,
				sizeof(PacketLayer::SC_Error_Msg), (char*)& errResponse);

			return;
		}


		// 해당 유저가 속해있는 로비 인덱스
		auto Idx = client->GetLobbyIndex();
		auto lobby = m_LobbyMgr->GetLobby(Idx);

		if (lobby == nullptr)
		{
			// TODO 해당 유저에게 알려야함 
			std::cout << "lobby is not exist...!" << std::endl;
		}
		
		// 받은 메세지를  ID + 보낼 메세지로 조합해서 로비객체가 
		// 로비가 브로드 캐스팅 하게 한다. 

		//memcpy(&sendPacket.sendID[0], client->GetID().c_str(), PacketLayer::MAX_USER_ID_SIZE);
		//memcpy(&sendPacket.msg[0], packet.pRefData, sizeof(PacketLayer::CS_Lobby_Chat_Pkt));
		
		lobby->BroadCastClient(client->GetClientIndex(),
			client->GetID().c_str(), recvPacket->msg);


	}

}