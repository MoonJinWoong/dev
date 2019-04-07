#pragma once
#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include "NetErrSet.h"

namespace NetworkLib
{
	class SelectNetwork
	{
	public:
		SelectNetwork();
		~SelectNetwork();
		NET_ERR_SET Init();
	private:
		SOCKET m_ServerSockfd;
	protected:
	};
}