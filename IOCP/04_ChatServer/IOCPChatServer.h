#pragma once

#include "ConnectionManager.h"

class ChatServer : public IOCPServer
{
private:
	SOCKET c_listenSocket;
public:

	ChatServer(void);
	~ChatServer(void);

	bool BindAndListen(int port);

	virtual bool OnAccept(Connection * lpConnection) override;
	virtual bool OnRecv(Connection * lpConnection, DWORD dwSize, char * pRecvedMsg) override;
	virtual bool OnRecvImmediately(Connection * lpConnection, DWORD dwSize, char * pRecvedMsg) override;
	virtual void OnClose(Connection * lpConnection) override;
	virtual bool OnSystemMsg(Connection * lpConnection, DWORD dwMsgType, LPARAM lParam) override;


};