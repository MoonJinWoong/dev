#include "ChatServer.h"


void ChatServer::OnAccept(c_u_Int unique_id)
{
	// 접속할때마다 큐에 밀어넣는다.
	PacketFrame packet{ unique_id ,(u_Short)PACKET_TYPE::CONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
	std::cout << "[OnAccept] UniqueID : " << unique_id << std::endl;
}

void ChatServer::OnClose(c_u_Int unique_id)
{
	// 세션이 접속끊을때마다 큐에 밀어넣는다.
	PacketFrame packet{ unique_id ,(u_Short)PACKET_TYPE::DISCONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
	std::cout << "[OnClose] UniqueID : " << unique_id << std::endl;
}

void ChatServer::OnRecv(c_u_Int unique_id, c_u_Int len, char* msg)
{
	// recv 올때마다 로직에서 받아서 처리한 후 인덱스를 큐에 밀어넣는다.
	mLogicProc->RecvPktData(unique_id, len, msg);
	std::cout << "[On recv] Byte : " << len << ", Msg :" << msg << std::endl;
}

void ChatServer::Run(u_Int maxClient)
{
	// logic에서 Send를 부르면 실행
	auto SendFunc = [&](unsigned int index, unsigned short size, char* pPacket)
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

