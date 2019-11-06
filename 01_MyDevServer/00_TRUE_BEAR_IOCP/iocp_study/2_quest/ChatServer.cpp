
#include "ChatServer.h"



void ChatServer::OnAccept(const unsigned int unique_id)
{
	printf("[OnConnect] Ŭ���̾�Ʈ: Index(%d)\n", unique_id);
}

void ChatServer::OnClose(const unsigned int unique_id)
{
	printf("[OnClose] Ŭ���̾�Ʈ: Index(%d)\n", unique_id);
}

void ChatServer::OnRecv(const unsigned int unique_id, const unsigned int len, char* msg)
{
	printf("[����] bytes : %d , msg : %s\n", len, msg);
}

void ChatServer::Run(unsigned int maxClient)
{
	// ���� ��Ŷ �Ŵ����� ���� ��.
	mPktMgr = std::make_unique<PacketMgr>();
	mPktMgr->Init(maxClient);
	mPktMgr->Start();

	StartNetService(maxClient);
}
