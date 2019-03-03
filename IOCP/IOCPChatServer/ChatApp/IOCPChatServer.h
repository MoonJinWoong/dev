#pragma once
#include "Connection.h"
#include "PacketProtocol.h"


class IOCPChatServer : IocpServer
{
public:
	IOCPChatServer();
	~IOCPChatServer();
	//client�� ���� ������ �Ǿ��� �� ȣ��Ǵ� �Լ�
	virtual	bool	OnAccept(Connection *lpConnection);
	//client���� packet�� �������� �� ���� �� �ְ� ó���Ǿ����� ��Ŷó��
	virtual	bool	OnRecv(Connection* lpConnection, DWORD dwSize, char* pRecvedMsg);
	//client���� packet�� �������� �� ���� �� ���� ��ٷ� ó�� �Ǵ� ��Ŷó��
	virtual	bool	OnRecvImmediately(Connection* lpConnection, DWORD dwSize, char* pRecvedMsg);
	//client�� ������ ����Ǿ��� �� ȣ��Ǵ� �Լ�
	virtual	void	OnClose(Connection* lpConnection);
};