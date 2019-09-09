#include "ExOverlappedIO.h"
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
	ExOverlappedIO* acceptContext = new ExOverlappedIO(this,IOType::IO_ACCEPT);
	DWORD bytes = 0;
	DWORD flags = 0;
	acceptContext->mWsaBuf.len = 0;
	acceptContext->mWsaBuf.buf = nullptr;

	if (FALSE == AcceptEx(
			*gIocpService->GetListenSocket(),
			mSocket,
			gIocpService->mAcceptBuf,
			0,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&bytes,
			(LPOVERLAPPED)acceptContext)
		)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			DeleteIoContext(acceptContext);
			printf_s("AcceptEx Error : %d\n", GetLastError());
			return false;
		}
	}

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

void Session::AddRef()
{
	if (InterlockedIncrement(&mRefCount) < 0)
	{
		std::cout << "add Session ref fail" << std::endl;
	}
}

void Session::ReleaseRef()
{
	if(InterlockedDecrement(&mRefCount) < 0)
	{
		std::cout << "release Session ref fail" << std::endl;
	}
}
