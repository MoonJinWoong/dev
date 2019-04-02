#pragma once
#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>

//#include <vector>
//#include <deque>
//#include <unordered_map>

#include "NetworkFrame.h"

namespace NetworkLayer
{
	class SelectNetwork : public NetworkFrame
	{
	public:
		SelectNetwork();
		virtual ~SelectNetwork();
		virtual bool Init(const ServerConfig& config);

		bool InitServerSocket();
		//bool BindAndListen();

	private:
		ServerConfig m_pConfig;
		SOCKET mServerSock;
	};
}