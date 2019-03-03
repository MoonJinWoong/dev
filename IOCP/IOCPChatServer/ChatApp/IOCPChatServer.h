#pragma once
#include "Connection.h"
#include "PacketProtocol.h"


class IOCPChatServer : IocpServer
{
public:
	IOCPChatServer();
	~IOCPChatServer();
	//client가 접속 수락이 되었을 때 호출되는 함수
	virtual	bool	OnAccept(Connection *lpConnection);
	//client에서 packet이 도착했을 때 순서 성 있게 처리되어지는 패킷처리
	virtual	bool	OnRecv(Connection* lpConnection, DWORD dwSize, char* pRecvedMsg);
	//client에서 packet이 도착했을 때 순서 성 없이 곧바로 처리 되는 패킷처리
	virtual	bool	OnRecvImmediately(Connection* lpConnection, DWORD dwSize, char* pRecvedMsg);
	//client와 연결이 종료되었을 때 호출되는 함수
	virtual	void	OnClose(Connection* lpConnection);
};