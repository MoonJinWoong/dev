
#include "ChatServer.h"



void ChatServer::OnConnect(const unsigned int index)
{

}

void ChatServer::OnClose(const unsigned int index)
{

}

void ChatServer::OnRecv(const unsigned int index, const unsigned int size_, char* pData_)
{

}

void ChatServer::Run(unsigned int maxClient)
{
	// ���⼭���� �ٽ�
	StartServer(maxClient);
}
