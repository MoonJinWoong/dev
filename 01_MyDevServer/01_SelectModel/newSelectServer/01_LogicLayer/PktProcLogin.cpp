#include "../02_NetworkLayer/ErrorSet.h"
#include "../02_NetworkLayer/SelectNetwork.h"
#include "Client.h"
#include "ClientManager.h"

#include "PktProcMain.h"


namespace LogicLayer
{
	void PktProcMain::ProcessLogin(recvPacket packet)
	{
		// 2019.05.07 이후 로비 리스트 요청 구현해야함


		PacketLayer::SC_Login_Pkt resPkt;
	
		auto reqPkt = (PacketLayer::CS_Login_Pkt*)packet.pRefData;

		// user mgr 에 유저를 등록 한다. 
		auto addRet = m_ClientMgr->AddClient(packet.clientIdx, reqPkt->szID);
		if (addRet != 0)
		{
			std::cout << "addClient error...!" << std::endl;
			return;
			// 접속 끊어줄 방법을 구현해야함 

		}
		else
		{
			strcpy(resPkt.msg, "loginSuccess");
			m_pSelecNetObj->SendInLogic(packet.clientIdx, (short)PACKET_ID::SC_LOGIN,
				sizeof(PacketLayer::SC_Login_Pkt), (char*)& resPkt);
		}
		
	}
}