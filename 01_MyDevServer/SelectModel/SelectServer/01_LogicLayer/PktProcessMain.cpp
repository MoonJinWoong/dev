

#include "PktProcessMain.h"



namespace LogicLayer
{
	PktProcessMain::PktProcessMain() {}
	PktProcessMain::~PktProcessMain() {}
	void PktProcessMain::Init(SelNet* pNetObj, ClientManager* pClientMgr)
	{
		// network layer select 객체 얻어와서 초기화 
		// client manager 전달 -> 이거 없으면 클라이언트 매니저 사용을 못함
		m_RefSelectNetObj = pNetObj;
		m_pRefClientMgr = pClientMgr;
	}
	void PktProcessMain::Process(RecvPckInfo packet)
	{
		auto packetID = packet.PacketId;

		switch (packetID)
		{
		case (int)PktID::LOGIN_IN_REQ:
			this->ProcLogin(packet);
		case (int)PktID::LOGIN_IN_RES:
		default:
			break;

		}
	}
	void PktProcessMain::StateCheck() {}

}