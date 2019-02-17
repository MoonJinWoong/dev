#include "IOCPChatServer.h"


//IOCPChatServer::IOCPChatServer(void) 
//{
//	m_pIocpServer= NULL;
//}
//IOCPChatServer::~IOCPChatServer(void) 
//{
//	if (m_pInstance != NULL)
//		delete m_pInstance;
//}

IOCPChatServer::IOCPChatServer() {}
IOCPChatServer::~IOCPChatServer() {}

bool IOCPChatServer::OnAccept(Connection* lpConnection)
{
	
	cout << "OnAccept ȣ�� " << endl;
	cout << "���� Ŭ�� ip �ּ� -"<<lpConnection->GetConnectionIp() << endl;		 
	//cout << "���� ���� size -"<<lpConnection->GetSendBufSize() << endl;
	//cout << "recv buf size-" << lpConnection->GetRecvBufSize() << endl;
	//cout<<"operation -> "<<lpConnection->s_e

	g_ConnectionManager()->AddConnection(lpConnection);
	return true;
}

bool IOCPChatServer::OnRecv(Connection* lpConnection, DWORD dwSize, char * pRecvMsg)
{
	cout << "OnRecv ȣ��Ǿ���....? " << endl;

	unsigned short usType;
	CopyMemory(&usType, pRecvMsg + 4, PACKET_TYPE_LENGTH);
	switch (usType)
	{
	case  PACKET_CHAT:
	{
		Packet_Chat* pChat = (Packet_Chat*)pRecvMsg;
		g_ConnectionManager()->BroadCast_Chat(pChat->s_szIP, pChat->s_szChatMsg);
		break;
	}
	default:
		break;
	}
	return true;
}
bool IOCPChatServer::OnRecvImmediately(Connection* lpConnection, DWORD dwSize, char * pRecvMsg)
{
	cout << "OnRecvImmediately ȣ��Ǿ���....? " << endl;
	
	unsigned short usType;
	CopyMemory(&usType, pRecvMsg + 4, PACKET_TYPE_LENGTH);
	switch (usType)
	{
	case  PACKET_CHAT:
	{
		Packet_Chat* pChat = (Packet_Chat*)pRecvMsg;
		g_ConnectionManager()->BroadCast_Chat(pChat->s_szIP, pChat->s_szChatMsg);
		break;
	}
	default:
		break;
	}
	return true;
}
void IOCPChatServer::OnClose(Connection* lpConnection)
{
	cout << "OnClose ȣ��Ǿ���....? " << endl;
}
bool IOCPChatServer::OnSystemMsg(Connection* lpConnection, DWORD dwMsgType, LPARAM lParam)
{
	cout << "OnSystemMsg ȣ��Ǿ���....? " << endl;
	return true;
}

bool IOCPChatServer::BindAndListen(int port)
{
	//���� ���� �Լ�. ���� ���� �� ��
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