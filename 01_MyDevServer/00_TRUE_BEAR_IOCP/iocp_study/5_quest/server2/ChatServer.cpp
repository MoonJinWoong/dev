#include "ChatServer.h"


void ChatServer::OnAccept(u_Int unique_id)
{
	// �����Ҷ����� ť�� �о�ִ´�.
	auto id = unique_id;
	std::cout << "[OnAccept] UniqueID : " << id << std::endl;
	PacketFrame packet{ id ,(int)PACKET_TYPE::CONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
}

void ChatServer::OnClose(u_Int unique_id)
{
	// ������ ���Ӳ��������� ť�� �о�ִ´�.
	PacketFrame packet{ unique_id ,(int)PACKET_TYPE::DISCONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
}

void ChatServer::OnRecv(u_Int unique_id, u_Int len, char* msg)
{

	// recv �ö����� �������� �޾Ƽ� ó���� �� �ε����� ť�� �о�ִ´�.
	mLogicProc->RecvPktData(unique_id, len, msg);
}

void ChatServer::Run(u_Int maxClient)
{
	// logic���� Send�� �θ��� ����
	auto SendFunc = [&](u_Int index, int size, char* pPacket)
	{
		SendMsg(index, size, pPacket);
	};

	mLogicProc = std::make_unique<LogicMain>();
	mLogicProc->SendPacketFunc = SendFunc;
	mLogicProc->Init(maxClient);
	mLogicProc->Start();
	StartNetService(maxClient);
}

void ChatServer::Stop()
{
	mLogicProc->Stop();
	DestroyThread();
}

