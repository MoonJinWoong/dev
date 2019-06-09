
#include "Session.h"


namespace NetworkLayer
{
	Session::Session() {}
	Session::~Session() {}
	void Session::Init()
	{
		m_Unique_id = -1;
		ZeroMemory(&m_stRecvIO, sizeof(SessionIO));
		ZeroMemory(&m_stSendIO, sizeof(SessionIO));
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