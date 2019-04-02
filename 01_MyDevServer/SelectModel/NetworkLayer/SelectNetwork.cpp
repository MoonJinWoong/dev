#include <stdio.h>
#include <string>
#include <iostream>
#include "SelectNetwork.h"

namespace NetworkLayer
{
	SelectNetwork::SelectNetwork() {}

	SelectNetwork::~SelectNetwork()
	{

	}

	bool SelectNetwork::Init(const ServerConfig& config)
	{
		 int port = config.Port;

		 // socket init
		 auto ret = InitServerSocket();
		 if (ret < 0)
			 std::cout << "Init Server failed..!" << std::endl;
		return true;
	}

	bool SelectNetwork::InitServerSocket()
	{
		std::cout << "けいしぉいけけぉい" << std::endl;
		WORD ver = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(ver, &wsaData);

		mServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mServerSock < 0)
			return false;
		

		auto n = 1;
		if (setsockopt(mServerSock, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(n)) < 0)
			return false;

		std::cout << "Init ServerSocket is Success...!" << std::endl;
		return true;
	}
}