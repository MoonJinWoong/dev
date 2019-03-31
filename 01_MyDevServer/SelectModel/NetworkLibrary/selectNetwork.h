#pragma once

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "NetworkFrame.h"


namespace NetworkLib
{
	class SelectNetwork :public NetworkFrame
	{
	public:
		SelectNetwork();
		~SelectNetwork();
		bool Init(const ServerOption &opt);
		bool InitSocket();
		void Start() override;
		void Stop() override;

	private:
		SOCKET m_ServerSock;
	};
}