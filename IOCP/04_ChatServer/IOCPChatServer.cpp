#include "IOCPChatServer.h"



ChatServer::ChatServer(void) {}
ChatServer::~ChatServer(void) {}

bool ChatServer::OnAccept(Connection* lpConnection)
{
	
	cout << "OnAccept ȣ�� " << endl;
	cout << "���� Ŭ�� ip �ּ� -"<<lpConnection->GetConnectionIp() << endl;		 
	cout << "���� ���� size -"<<lpConnection->GetSendBufSize() << endl;

	return true;
}
bool ChatServer::OnRecv(Connection* lpConnection, DWORD dwSize, char * pRecvMsg)
{
	cout << "OnRecv ȣ��Ǿ���....? " << endl;
	return true;
}
bool ChatServer::OnRecvImmediately(Connection* lpConnection, DWORD dwSize, char * pRecvMsg)
{
	cout << "OnRecvImmediately ȣ��Ǿ���....? " << endl;
	return true;
}
void ChatServer::OnClose(Connection* lpConnection)
{
	cout << "OnClose ȣ��Ǿ���....? " << endl;
}
bool ChatServer::OnSystemMsg(Connection* lpConnection, DWORD dwMsgType, LPARAM lParam)
{
	cout << "OnSystemMsg ȣ��Ǿ���....? " << endl;
	return true;
}

bool ChatServer::BindAndListen(int port)
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;

	// ���� ��Ʈ ����
	stServerAddr.sin_port = htons(port);

	// � �ּҿ��� ������ �����̶� �޾Ƶ��̰ڴ�. 
	// ���� ������� �̷��� �����Ѵ�. ���࿡ �ϳ��� �����ǿ����� ������ �ް� ������ 
	// �� �ּҸ� inet_addr �Լ��� �̿��� ������ �ȴ�. 
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// ������ ������ ���� �ּ� ������ cIOCmpletionPort ������ �����Ѵ�. 

	int nRet = bind(c_listenSocket, (SOCKADDR*)&stServerAddr,
		sizeof(SOCKADDR_IN));

	if (nRet != 0)
	{
		printf("error!!! bind() failed !! %d\n", WSAGetLastError());
		return false;
	}

	// ���� ��û�� �޾Ƶ��̱� ���� clCompletionPort ������ ����ϰ�
	// ���� ���ť�� 5���� �����Ѵ�. 
	nRet = listen(c_listenSocket, 5);
	if (nRet != 0)
	{
		printf("error !!! listen() failed !!! %d\n", WSAGetLastError());
		return false;
	}

	printf("Server bind and listen success!!!!! \n");
	return true;
}