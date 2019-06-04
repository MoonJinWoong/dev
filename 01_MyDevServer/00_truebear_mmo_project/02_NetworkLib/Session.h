#pragma once

#include "Singleton.h"


namespace NetworkLayer
{
	struct CLIENT_INFO
	{
		SOCKET s_sock;
		SOCKADDR_IN addrInfo;
	};

	class Session
	{
	public:
		Session();
		virtual ~Session();

		bool ProcAccept(SOCKET sock, SOCKADDR_IN addrLen);
		
		void ProcSend(size_t size);
		void SendData();


	private:

	};
}