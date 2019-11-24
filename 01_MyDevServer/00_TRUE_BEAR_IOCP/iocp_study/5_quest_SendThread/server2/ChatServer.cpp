#include "ChatServer.h"


void ChatServer::OnAccept(u_Int unique_id)
{
	// 접속할때마다 큐에 밀어넣는다.
	auto id = unique_id;
	std::cout << "[OnAccept] UniqueID : " << id << std::endl;
	PacketFrame packet{ id ,(int)PACKET_TYPE::CONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
}

void ChatServer::OnClose(u_Int unique_id)
{
	// 세션이 접속끊을때마다 큐에 밀어넣는다.
	PacketFrame packet{ unique_id ,(int)PACKET_TYPE::DISCONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
}

void ChatServer::OnRecv(u_Int unique_id, u_Int len, char* msg)
{

	// recv 올때마다 로직에서 받아서 처리한 후 인덱스를 큐에 밀어넣는다.
	mLogicProc->RecvPktData(unique_id, len, msg);
}

void ChatServer::Run(u_Int maxClient)
{
	// logic에서 Send를 부르면 실행
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

