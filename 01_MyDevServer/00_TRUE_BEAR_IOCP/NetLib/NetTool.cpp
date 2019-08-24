#include "NetTool.h"


NetTool::NetTool()
{
	mhSock = -1;
	ZeroMemory(&mReadOverLappedEx.overlapped, sizeof(mReadOverLappedEx.overlapped));
}

NetTool::NetTool(SocketType type)
{
	// wsastartup , ���� ���� , �������� �ʱ�ȭ 
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	if (type == SocketType::TCP)
	{
		mhSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	}
	else
	{
		mhSock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	}

	// �̰� ���ϸ� socket error ����. 3�ð����� ������
	ZeroMemory(&mReadOverLappedEx.overlapped, sizeof(mReadOverLappedEx.overlapped));
}

NetTool::~NetTool()
{

}

void NetTool::bindAndListen(const string& ip, int port)
{
	cout << "[INFO] IP :" << ip << endl;
	cout << "[INFO] port:" << port << endl;

	// ���� ���� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	// bind 
	if (bind(mhSock, (sockaddr*)& serverAddr, sizeof(SOCKADDR_IN)) < 0)
	{
		cout << "bind error "<<endl;
		closesocket(mhSock);
		WSACleanup();
	}
	else
	{
		cout << "[INFO] bind success" << endl;
	}


	// listen
	if (listen(mhSock, 5000) < 0)
	{
		cout << "listen error " << endl;
		closesocket(mhSock);
		WSACleanup();
	}
	else
	{
		cout << "[INFO] listen success" << endl;

	}
}

bool NetTool::AsyncAccept(NetTool& candidate)
{
	// �Լ� ������ ������. �̷��� ������
	if (AcceptEx == NULL)
	{
		DWORD bytes;
		WSAIoctl(mhSock,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&UUID(WSAID_ACCEPTEX),
			sizeof(UUID),
			&AcceptEx,
			sizeof(AcceptEx),
			&bytes,
			NULL,
			NULL);

		if (AcceptEx == NULL)
		{
			cout << "Accept Ex get fail" << endl;
			return false;
		}
	}

	// ������ �����ּҿ� ����Ʈ�ּ�
	char addr[200];
	DWORD flag = 0;

	bool ret = AcceptEx(mhSock,
		candidate.mhSock,
		&addr,
		0,
		50,
		50,
		&flag,
		&mReadOverLappedEx.overlapped
	) == TRUE;

	return ret;
}
