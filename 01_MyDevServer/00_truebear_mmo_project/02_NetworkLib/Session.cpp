
#include "Session.h"


namespace Network
{
	Session::Session() {}
	Session::~Session() {}
	void Session::Init()
	{
		m_Unique_id = -1;
		ZeroMemory(&m_stRecvIO, sizeof(ExOverIo));
		ZeroMemory(&m_stSendIO, sizeof(ExOverIo));
		m_socket = INVALID_SOCKET;
	}
	bool Session::OnAccept(SOCKET sock, SOCKADDR_IN addrInfo)
	{
		m_socket = sock;
		int addrLen = 0;
		getpeername(m_socket, (struct sockaddr*)
			& addrInfo, &addrLen);
		return true;
	}
}