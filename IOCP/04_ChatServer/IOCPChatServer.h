#pragma once

#include "ConnectionManager.h"

class IOCPChatServer : public IOCPServer
{
	//DECLEAR_SINGLETON(IOCPChatServer);

public:

	IOCPChatServer(void);
	~IOCPChatServer(void);

	bool BindAndListen(int port);

	// client 접속 수락 되었을 때 호출
	virtual bool OnAccept(Connection * lpConnection) override;
	// client에서 packet이 도착 했을 때 순서 있게 처리
	virtual bool OnRecv(Connection * lpConnection, DWORD dwSize, char * pRecvedMsg) override;
	
	// client 에서 packet이 도착했을 때 순서 없이 처리
	virtual bool OnRecvImmediately(Connection * lpConnection, DWORD dwSize, char * pRecvedMsg) override;
	virtual void OnClose(Connection * lpConnection) override;
	virtual bool OnSystemMsg(Connection * lpConnection, DWORD dwMsgType, LPARAM lParam) override;

private:
	//IOCPServer* m_piocpServer;
	SOCKET c_listenSocket;
};

//CREATE_FUNCTION(IOCPChatServer, chatserver);