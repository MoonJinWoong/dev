#include "NetObject.h"

NetObject::NetObject()
{
	// 오버랩드 초기화 잊지말고 할것.
}

NetObject::~NetObject()
{

}

void NetObject::MakeSocket()
{
	mSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	// 오버랩드 메모리 초기화
}

void NetObject::MakeSocket(SOCKET socket)
{
	mSock = socket;
	// 오버랩드 메모리 초기화
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
