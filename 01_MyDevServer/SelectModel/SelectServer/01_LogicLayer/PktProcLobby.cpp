#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"
#include "../02_NetworkLayer/SelectNetwork.h"

#include "Client.h"
#include "ClientManager.h"
#include "LobbyManager.h"
#include "Lobby.h"

#include "PktProcessMain.h"

using PACKE_ID = PacketLayer::PACKET_ID;
using PktLobbyListRes = PacketLayer::PktLobbyListRes;
using PktLobbyEnterRes = PacketLayer::PktLobbyEnterRes;


namespace LogicLayer
{
	void PktProcessMain::ProcLobbyList(RecvPckInfo packetInfo)
	{
		// 인증 받은 유저인가?
		// 아직 로비에 들어가지 않은 유저인가?
		auto pClient = m_pRefClientMgr->GetClient(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pClient);

		if (errorCode != LOGIC_ERROR_SET::NONE)
		{
			std::cout << "logic error -> " << (short)errorCode << std::endl;
			return;
		}
			
		auto pClientData = std::get<1>(pClient);

		if (pClientData->IsCurDomainInLogIn() == false) 
		{
			std::cout << "logic error -> " << (short)LOGIC_ERROR_SET::LOBBY_LIST_INVALID << std::endl;
			return;
		}
			
		// 으어어어어어 졸리다ㅏㅏㅏㅏㅏㅏㅏㅏ
		auto k = m_pRefLobbyMgr->SendLobbyListInfo(packetInfo.SessionIndex);
		
		
		if (k != NET_ERROR_SET::NONE)
			std::cout << "logic Send failed PrcLogin...!" << std::endl;
		
	}

	void PktProcessMain::ProcLobbyEnter(RecvPckInfo packetInfo)
	{
		auto reqPkt = (PacketLayer::PktLobbyEnterReq*)packetInfo.pRefData;
		PktLobbyEnterRes resPkt;

		auto pClientRet = m_pRefClientMgr->GetClient(packetInfo.SessionIndex);
		auto err = std::get<0>(pClientRet);

		if (err != LOGIC_ERROR_SET::NONE)
			std::cout << "logic error -> " << (short)err << std::endl;

		auto pClient = std::get<1>(pClientRet);

		if (pClient->IsCurDomainInLogIn() == false) 
			std::cout << "logic error -> " << (short)LOGIC_ERROR_SET::LOBBY_ENTER_INVALID_DOMAIN << std::endl;
		

		auto pLobby = m_pRefLobbyMgr->GetLobby(reqPkt->selectedLobbyID);
		if (pLobby == nullptr) 
			 std::cout << "logic error -> " << (short)LOGIC_ERROR_SET::LOBBY_ENTER_INVALID_LOBBY_INDEX << std::endl;
		
		
		auto enterRet = pLobby->Enter(pClient);
		if (enterRet != LOGIC_ERROR_SET::NONE)
		{
			std::cout << "logic error -> " << (short)LOGIC_ERROR_SET::LOBBY_ENTER_EMPTY_USER_LIST << std::endl;
		}
			
		

		auto ret = m_RefSelectNetObj->LogicSendBufferSet(packetInfo.SessionIndex,
			(short)PACKE_ID::LOBBY_ENTER_RES, sizeof(PktLobbyEnterRes), (char*)& resPkt);

		if (ret != NET_ERROR_SET::NONE)
		{
			std::cout << "logic Send failed LobbyEnter...!" << std::endl;
		}
	}
}