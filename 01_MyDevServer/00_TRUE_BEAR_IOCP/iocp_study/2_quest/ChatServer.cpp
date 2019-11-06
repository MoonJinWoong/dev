
#include "ChatServer.h"



void ChatServer::OnAccept(const unsigned int unique_id)
{
	printf("[OnConnect] 클라이언트: Index(%d)\n", unique_id);
}

void ChatServer::OnClose(const unsigned int unique_id)
{
	printf("[OnClose] 클라이언트: Index(%d)\n", unique_id);
}

void ChatServer::OnRecv(const unsigned int unique_id, const unsigned int len, char* msg)
{
	printf("[수신] bytes : %d , msg : %s\n", len, msg);
}

void ChatServer::Run(unsigned int maxClient)
{
	// 질문 패킷 매니저에 관한 것.
	mPktMgr = std::make_unique<PacketMgr>();
	mPktMgr->Init(maxClient);
	mPktMgr->Start();

	StartNetService(maxClient);
}
