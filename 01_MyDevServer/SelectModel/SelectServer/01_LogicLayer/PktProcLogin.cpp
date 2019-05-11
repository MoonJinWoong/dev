#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"
#include "../02_NetworkLayer/SelectNetwork.h"
//#include "ConnectedUserManager.h"
#include "Client.h"
#include "ClientManager.h"
#include "LobbyManager.h"
#include "PktProcessMain.h"


using PACKE_ID = PacketLayer::PACKET_ID;
using PktLoginResponse = PacketLayer::PktLogInRes;


namespace LogicLayer
{
	class ClientManager;

	void PktProcessMain::ProcLogin(RecvPckInfo packetInfo)
	{
		PacketLayer::PktLogInRes resPkt;
		auto requestPkt = (PacketLayer::PktLogInReq*)packetInfo.pRefData;

		auto addRet = m_pRefClientMgr->Add(packetInfo.SessionIndex, requestPkt->szID);
		


		// TODO 중복 코드 제거 해야함
		if (addRet == false)
		{
			std::cout << "ClientManager Add is failed..!" << std::endl;
			PacketLayer::FailMsg resFail;
			strcpy(resFail.msg, "LoginFail");
			
			auto ret = m_RefSelectNetObj->LogicSendBufferSet(packetInfo.SessionIndex, (short)PACKE_ID::LOGIN_IN_RES
				, sizeof(PktLoginResponse), (char*)& resPkt);
			if (ret != NET_ERROR_SET::NONE)
				std::cout << "logic Send failed PrcLogin...!" << std::endl;
		}
		else 
		{
			strcpy(resPkt.msg, "LoginSuccess");
			auto ret = m_RefSelectNetObj->LogicSendBufferSet(packetInfo.SessionIndex, (short)PACKE_ID::LOGIN_IN_RES
				, sizeof(PktLoginResponse), (char*)& resPkt);
			if (ret != NET_ERROR_SET::NONE)
				std::cout << "logic Send failed PrcLogin...!" << std::endl;
		}





	}
}