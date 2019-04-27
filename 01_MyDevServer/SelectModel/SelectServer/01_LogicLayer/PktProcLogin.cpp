
#include "../02_NetworkLayer/SelectNetwork.h"
#include "Client.h"
#include "ClientManager.h"
#include "PktProcessMain.h"
#include <iostream>



namespace LogicLayer
{
	class ClientManager;

	void PktProcessMain::ProcLogin(RecvPckInfo packetInfo)
	{
		PacketLayer::PktLogInRes packet;
		auto reqPkt = (PacketLayer::PktLogInReq*)packetInfo.pRefData;

		auto addRet = m_pRefClientMgr->Add(packetInfo.SessionIndex, reqPkt->szID);
		
		if (addRet == false)
			std::cout << "ClientManager Add is failed..!" << std::endl;


		// 여기서부터 다시 
		// 로직단에서 처리한걸 네트워크로 실어 보낼때 구조를 어찌할 것인가
		// send
		m_RefSelectNetObj->

	}
}