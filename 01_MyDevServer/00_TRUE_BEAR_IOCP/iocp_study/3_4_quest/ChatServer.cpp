#include "ChatServer.h"


void ChatServer::OnAccept(const unsigned int unique_id)
{
	// 접속할때마다 큐에 밀어넣는다.
	PacketFrame packet{ unique_id ,(unsigned short)PACKET_TYPE::CONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
	std::cout << "[OnAccept] UniqueID : " << unique_id << std::endl;
}

void ChatServer::OnClose(const unsigned int unique_id)
{
	// 접속끊을때마다 큐에 밀어넣는다.
	PacketFrame packet{ unique_id ,(unsigned short)PACKET_TYPE::DISCONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
	std::cout << "[OnClose] UniqueID : " << unique_id << std::endl;
}

void ChatServer::OnRecv(const unsigned int unique_id, const unsigned int len, char* msg)
{
	// recv 올때마다 로직에서 받아서 처리한다.
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
