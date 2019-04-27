

#include "PktProcessMain.h"



namespace LogicLayer
{
	PktProcessMain::PktProcessMain() {}
	PktProcessMain::~PktProcessMain() {}
	void PktProcessMain::Init(SelNet* pNetObj)
	{
		// network layer select ��ü ���ͼ� �ʱ�ȭ 
		m_RefSelectNetObj = pNetObj;
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