

#include "PktProcessMain.h"



namespace LogicLayer
{
	PktProcessMain::PktProcessMain() {}
	PktProcessMain::~PktProcessMain() {}
	void PktProcessMain::Init(SelNet* pNetObj, ClientManager* pClientMgr)
	{
		// network layer select ��ü ���ͼ� �ʱ�ȭ 
		// client manager ���� -> �̰� ������ Ŭ���̾�Ʈ �Ŵ��� ����� ����
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
		case (int)PktID::LOBBY_LIST_REQ:
			this->ProcLobbyList(packet);
		default:
			break;

		}
	}
	void PktProcessMain::StateCheck() {}

}