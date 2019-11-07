#include "ChatServer.h"


void ChatServer::OnAccept(const unsigned int unique_id)
{
	// �����Ҷ����� ť�� �о�ִ´�.
	PacketFrame packet{ unique_id ,(unsigned short)PACKET_TYPE::CONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
	std::cout << "[OnAccept] UniqueID : " << unique_id << std::endl;
}

void ChatServer::OnClose(const unsigned int unique_id)
{
	// ���Ӳ��������� ť�� �о�ִ´�.
	PacketFrame packet{ unique_id ,(unsigned short)PACKET_TYPE::DISCONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
	std::cout << "[OnClose] UniqueID : " << unique_id << std::endl;
}

void ChatServer::OnRecv(const unsigned int unique_id, const unsigned int len, char* msg)
{
	// recv �ö����� �������� �޾Ƽ� ó���Ѵ�.
	mLogicProc->RecvPktData(unique_id, len, msg);
	std::cout << "[On recv] Byte : " << len << ", Msg :" << msg << std::endl;
}

void ChatServer::Run(unsigned int maxClient)
{

	mLogicProc = std::make_unique<LogicMain>();
	mLogicProc->Init(maxClient);
	mLogicProc->Start();

	StartNetService(maxClient);
}
