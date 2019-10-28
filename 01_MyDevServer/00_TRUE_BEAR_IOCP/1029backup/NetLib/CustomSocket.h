#pragma once
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib")
#include <WinSock2.h> // memo - window.h 보다 위에 있어야함
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>


class CustomSocket
{
public:
	CustomSocket() { mSock = INVALID_SOCKET; }
	~CustomSocket() {}

	void CreateSocket()
	{
		mSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (mSock == SOCKET_ERROR)
		{
			std::cout << "fail WSASocket " << std::endl;
		}
	}
	void CreateSocket(SOCKET sock)
	{
		mSock = sock;
	}

	void BindAndListenSocket()
	{
		SOCKADDR_IN	addr;
		ZeroMemory(&addr, sizeof(SOCKADDR_IN));

		addr.sin_family = AF_INET;
		addr.sin_port = htons(8000);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (::bind(mSock, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == SOCKET_ERROR)
		{
			std::cout<<"fail bind "<<std::endl;
			return;
		}

		if (::listen(mSock, SOMAXCONN) == SOCKET_ERROR)
		{
			std::cout << "fail listen " << std::endl;
			return;
		}
	}

public:
	SOCKET mSock;

};