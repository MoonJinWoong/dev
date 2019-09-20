#include "NetObject.h"

NetObject::NetObject()
{
	// �������� �ʱ�ȭ �������� �Ұ�.
}

NetObject::~NetObject()
{

}

void NetObject::MakeSocket()
{
	mSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	// �������� �޸� �ʱ�ȭ
}

void NetObject::MakeSocket(SOCKET socket)
{
	mSock = socket;
	// �������� �޸� �ʱ�ȭ
}

bool NetObject::BindAndListen(const EndAddress& addr)
{

	// bind 
	auto ret = ::bind(
		mSock,
		(sockaddr*)& addr.m_ipv4EndAddr,
		sizeof(addr.m_ipv4EndAddr));
	if (ret < 0)
	{
		std::cout << "bind fail.... uu" << std::endl;
		return false;
	}

	// listen
	auto ret2 = listen(mSock, SOMAXCONN);
	if (ret2 == SOCKET_ERROR)
	{
		std::cout << "bind fail.... uu" << std::endl;
		return false;
	}

	return true;

}
