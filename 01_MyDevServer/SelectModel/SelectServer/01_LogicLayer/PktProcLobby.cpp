#include "../02_NetworkLayer/SelectNetwork.h"
#include "Client.h"
#include "ClientManager.h"
#include "PktProcessMain.h"
#include <iostream>
#include <tuple>
#include "LogicErrorSet.h"
#include "PktProcessMain.h"

using PACKE_ID = PacketLayer::PACKET_ID;
using PktLobbyListRes = PacketLayer::PktLobbyListRes;


namespace LogicLayer
{
	void PktProcessMain::ProcLobbyList(RecvPckInfo packetInfo)
	{
			// 인증 받은 유저인가?
			// 아직 로비에 들어가지 않은 유저인가?
		auto pClient = m_pRefClientMgr->GetClient(packetInfo.SessionIndex);
		auto errorCode = std::get<0>(pClient);

		if (errorCode != LOGIC_ERROR_SET::NONE)
			std::cout << "logic error -> " << (short)errorCode << std::endl;
		

		auto pClientData = std::get<1>(pClient);

		if (pClientData->IsCurDomainInLogIn() == false) 
			std::cout<<"logic error -> "<< (short)LOGIC_ERROR_SET::LOBBY_LIST_INVALID <<std::endl;
		

		m_pRefLobbyMgr->SendLobbyListInfo(packetInfo.SessionIndex);

		return ERROR_CODE::NONE;



		NCommon::PktLobbyListRes resPkt;
		resPkt.SetError(__result);
		m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::LOBBY_LIST_RES, sizeof(resPkt), (char*)& resPkt);
		return (ERROR_CODE)__result;
	}
}