#include "Session.h"

const int CLIENT_BUFSIZE = 65536;


Session::Session(): 
					 mSendBufSize(CLIENT_BUFSIZE), mRecvBufSize(CLIENT_BUFSIZE),
					 mConnected(0), mRefCount(0), mSendPendingCount(0)
{
	memset(&mClientAddr, 0, sizeof(SOCKADDR_IN));
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

Session::~Session()
{

}

void Session::SessionReset()
{

}

bool Session::PostAccept()
{


	return true;
}

bool Session::AcceptCompletion()
{
	
	return true;
}

void Session::OnDisconnect()
{
	
}

void Session::OnRelease()
{
}

