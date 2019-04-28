
#include "../02_NetworkLayer/SelectNetwork.h"
#include "Client.h"
#include "ClientManager.h"
#include "PktProcessMain.h"
#include <iostream>


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
		

		if (addRet == false)
		{
			std::cout << "ClientManager Add is failed..!" << std::endl;
			strcpy(resPkt.msg, "LoginFail");
		}
		else
			strcpy(resPkt.msg, "LoginSucc");
			

		m_RefSelectNetObj->LogicSendBufferSet(packetInfo.SessionIndex, (short)PACKE_ID::LOGIN_IN_RES
			, sizeof(PktLoginResponse), (char*)& resPkt);

	}
}