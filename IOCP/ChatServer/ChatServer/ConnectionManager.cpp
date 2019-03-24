

#include "preCompile.h"



ConnectionManager::ConnectionManager()
{

}
ConnectionManager::~ConnectionManager()
{

}

void ConnectionManager::SendPacket(SOCKET &s, unsigned char *packet)
{
	WSAOVERLAPPED_EX *send_over = new WSAOVERLAPPED_EX;
	ZeroMemory(send_over, sizeof(*send_over));
	send_over->event_type = E_SEND;
	memcpy(send_over->IOCP_buf, packet, packet[0]);
	send_over->wsabuf.buf = reinterpret_cast<CHAR *>(send_over->IOCP_buf);
	send_over->wsabuf.len = packet[0];
	DWORD send_flag = 0;
	WSASend(s, &send_over->wsabuf, 1, NULL, send_flag, &send_over->over, NULL);
	//cout << "send " << endl;
}