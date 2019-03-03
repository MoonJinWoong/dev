

#include "IOCPChatServer.h"


IOCPChatServer::IOCPChatServer()
{

}
IOCPChatServer::~IOCPChatServer()
{

}

bool OnAccept(Connection *lpConnection)
{
	
	Connection->AddConnection(lpConnection);
	cout << "connection succ...!!! " << endl;
	ConnectionManager()->GetConnectionCnt());

	return true;
}