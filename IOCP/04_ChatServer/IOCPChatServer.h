#pragma once

#include "ConnectionManager.h"

class IOCPChatServer : public IOCPServer
{
	//DECLEAR_SINGLETON(IOCPChatServer);

public:

	IOCPChatServer(void);
	~IOCPChatServer(void);

	bool BindAndListen(int port);

	// client ���� ���� �Ǿ��� �� ȣ��
	virtual bool OnAccept(Connection * lpConnection) override;
	// client���� packet�� ���� ���� �� ���� �ְ� ó��
	virtual bool OnRecv(Connection * lpConnection, DWORD dwSize, char * pRecvedMsg) override;
	
	// client ���� packet�� �������� �� ���� ���� ó��
	virtual bool OnRecvImmediately(Connection * lpConnection, DWORD dwSize, char * pRecvedMsg) override;
	virtual void OnClose(Connection * lpConnection) override;
	virtual bool OnSystemMsg(Connection * lpConnection, DWORD dwMsgType, LPARAM lParam) override;

private:
	//IOCPServer* m_piocpServer;
	SOCKET c_listenSocket;
};

//CREATE_FUNCTION(IOCPChatServer, chatserver);