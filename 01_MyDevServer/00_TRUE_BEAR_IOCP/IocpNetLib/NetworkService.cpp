#include "precompile.h"

NetworkService::NetworkService()
{
	mSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ZeroMemory(&m_readOverlappedStruct, sizeof(m_readOverlappedStruct));
}

NetworkService::~NetworkService()
{

}



void NetworkService::bindAndListen( int port)
{
	//  에러 처리 msdn 그대로 작성 
	//  TODO. 에러 클래스를 만들어서 cout 대신에 통일성 있게 출력하게 만들자
	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(port);
	ServerAddr.sin_addr.s_addr = INADDR_ANY;


	// bind 
	auto bindRet = ::bind(mSock, reinterpret_cast<sockaddr*>(&ServerAddr), sizeof(ServerAddr));
	if (bindRet == SOCKET_ERROR)
	{
		cout << "bind function failed with error - " << WSAGetLastError() << endl;
		auto ret = closesocket(mSock);
		if (ret == SOCKET_ERROR)
			cout << "closesocket function failed with error - " << WSAGetLastError() << endl;
		WSACleanup();
	}
	else
	{
		cout << "[SERVER] bind success " << endl;

	}


	//listen
	auto listenRet = listen(mSock, SOMAXCONN);
	if (listenRet == SOCKET_ERROR)
	{
		cout <<"listen function failed with error - " << WSAGetLastError() << endl;

		auto ret2 = closesocket(mSock);
		if (ret2 == SOCKET_ERROR)
		{
			cout << "closesocket function failed with error - " << WSAGetLastError() << endl;
			WSACleanup();
		}
	}
	else
	{
		cout << "[SERVER] listen success " << endl;
	}
		

}

void NetworkService::initAccpetEx()
{
	// 비동기 accept를 하겠다
	DWORD bytes = 0;
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	if (SOCKET_ERROR == WSAIoctl(mSock, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx, sizeof(GUID), &mFnAcceptEx, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL)) 
	{
		cout << "[ERR] WSAIoctl error in Init Accept " << endl;
	}
}
