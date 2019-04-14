#pragma once

#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>

#include <vector>
#include <deque>
#include <unordered_map>
#include <iostream>
#include "Defines.h"


namespace NetworkLayer
{
	class SelectNetwork
	{
	public:
		SelectNetwork();
		~SelectNetwork();
		bool Init();
		void Run();
		bool CheckNewClient();
		void SetSockOption(const SOCKET fd);

	private:
		SOCKET m_ServerSockfd;

		fd_set m_Readfds;
		CommonPkt m_packet;
	};
}