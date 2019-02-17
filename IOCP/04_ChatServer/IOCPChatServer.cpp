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
	
	cout << "OnAccept 호출 " << endl;
	cout << "접속 클라 ip 주소 -"<<lpConnection->GetConnectionIp() << endl;		 
	//cout << "전송 버퍼 size -"<<lpConnection->GetSendBufSize() << endl;
	//cout << "recv buf size-" << lpConnection->GetRecvBufSize() << endl;
	//cout<<"operation -> "<<lpConnection->s_e

	g_ConnectionManager()->AddConnection(lpConnection);
	return true;
}

bool IOCPChatServer::OnRecv(Connection* lpConnection, DWORD dwSize, char * pRecvMsg)
{
	cout << "OnRecv 호출되었니....? " << endl;

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
	cout << "OnRecvImmediately 호출되었니....? " << endl;
	
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
	cout << "OnClose 호출되었니....? " << endl;
}
bool IOCPChatServer::OnSystemMsg(Connection* lpConnection, DWORD dwMsgType, LPARAM lParam)
{
	cout << "OnSystemMsg 호출되었니....? " << endl;
	return true;
}

bool IOCPChatServer::BindAndListen(int port)
{
	//쓸모 없는 함수. 추후 삭제 할 것
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;

	// 서버 포트 설정
	stServerAddr.sin_port = htons(port);

	// 어떤 주소에서 들어오는 접속이라도 받아들이겠다. 
	// 보통 서버라면 이렇게 설정한다. 만약에 하나의 아이피에서만 접속을 받고 싶으면 
	// 그 주소를 inet_addr 함수를 이용해 넣으면 된다. 
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 위에서 지정한 서버 주소 정보와 cIOCmpletionPort 소켓을 연결한다. 

	int nRet = bind(c_listenSocket, (SOCKADDR*)&stServerAddr,
		sizeof(SOCKADDR_IN));

	if (nRet != 0)
	{
		printf("error!!! bind() failed !! %d\n", WSAGetLastError());
		return false;
	}

	// 접속 요청을 받아들이기 위해 clCompletionPort 소켓을 등록하고
	// 접속 대기큐를 5개로 세팅한다. 
	nRet = listen(c_listenSocket, 5);
	if (nRet != 0)
	{
		printf("error !!! listen() failed !!! %d\n", WSAGetLastError());
		return false;
	}

	printf("Server bind and listen success!!!!! \n");
	return true;
}