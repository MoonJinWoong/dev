#include "Session.h"

Session::Session()
	: mSendBuffer(SESSION_MAX_SEND_BUFF), mRecvBuffer(SESSION_MAX_RECV_BUFF)
	, mConnected(0), mRefCount(0), mSendPendingCount(0)
{
	memset(&mClientAddr, 0, sizeof(SOCKADDR_IN));
	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

Session::~Session()
{

}

void Session::Init()
{
	mConnected = 0;
	mRefCount = 0;
	memset(&mClientAddr, 0, sizeof(SOCKADDR_IN));

	mRecvBuffer.BufferReset();

	mSendBufferLock.Start();
	mSendBuffer.BufferReset();
	mSendBufferLock.End();

	LINGER lingerOption;
	lingerOption.l_onoff = 1;
	lingerOption.l_linger = 0;

	/// no TCP TIME_WAIT
	if (SOCKET_ERROR == setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)& lingerOption, sizeof(LINGER)))
	{
		printf_s("[DEBUG] setsockopt linger option error: %d\n", GetLastError());
	}
	closesocket(mSocket);

	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

void Session::AddCount()
{
	if(InterlockedIncrement(&mRefCount) <= 0)
	{
		std::cout << "Add Count err" << std::endl;
	}
}


bool Session::AcceptOverlapped(SOCKET &listenSocket, char * addr)
{

	CustomOverlapped* pOver = new CustomOverlapped(this,IOType::IO_ACCEPT);
	DWORD bytes = 0;
	DWORD flags = 0;
	pOver->mWsaBuf.len = 0;
	pOver->mWsaBuf.buf = nullptr;



	if (FALSE == AcceptEx(listenSocket, mSocket, addr, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, (LPOVERLAPPED)pOver))
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			//DeleteIoContext(acceptContext);
			printf_s("AcceptEx Error : %d\n", GetLastError());

			return false;
		}
	}

	return true;
}

void Session::AcceptFinish()
{

	
}
