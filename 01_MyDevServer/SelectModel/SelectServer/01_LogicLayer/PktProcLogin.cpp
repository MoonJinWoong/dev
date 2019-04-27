
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


		// ���⼭���� �ٽ� 
		// �����ܿ��� ó���Ѱ� ��Ʈ��ũ�� �Ǿ� ������ ������ ������ ���ΰ�
		// send
		m_RefSelectNetObj->

	}
}