#include "ExOverlappedIO.h"
#include "Session.h"



Session::Session(): 
					// mSendBufSize(CLIENT_BUFSIZE), mRecvBufSize(CLIENT_BUFSIZE),
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
	if (1 == InterlockedExchange(&mConnected, 1))
	{
		/// already exists?
		printf_s("[DEBUG]  already exists: \n");
		return false;
	}

	bool stateOpt = true;
	do
	{
		if (SOCKET_ERROR == setsockopt(
			mSocket, 
			SOL_SOCKET, 
			SO_UPDATE_ACCEPT_CONTEXT, 
			(char*)gIocpService->GetListenSocket(),
			sizeof(SOCKET)))
		{
			printf_s("[DEBUG] SO_UPDATE_ACCEPT_CONTEXT error: %d\n", GetLastError());
			stateOpt = false;
			break;
		}

		int opt = 1;
		if (SOCKET_ERROR == setsockopt(
			mSocket, 
			IPPROTO_TCP, 
			TCP_NODELAY, 
			(const char*)& opt, 
			sizeof(int)))
		{
			printf_s("[DEBUG] TCP_NODELAY error: %d\n", GetLastError());
			stateOpt = false;
			break;
		}

		opt = 0;
		if (SOCKET_ERROR == setsockopt(
			mSocket,
			SOL_SOCKET, 
			SO_RCVBUF, 
			(const char*)& opt, 
			sizeof(int)))
		{
			printf_s("[DEBUG] SO_RCVBUF change error: %d\n", GetLastError());
			stateOpt = false;
			break;
		}

		int addrlen = sizeof(SOCKADDR_IN);
		if (SOCKET_ERROR == getpeername(
			mSocket, 
			(SOCKADDR*)& mClientAddr,
			&addrlen))
		{
			printf_s("[DEBUG] getpeername error: %d\n", GetLastError());
			stateOpt = false;
			break;
		}



		HANDLE handle = CreateIoCompletionPort(
			(HANDLE)mSocket, 
			gIocpService->GetComletionPort(), 
			(ULONG_PTR)this,
			0);

		if (handle != gIocpService->GetComletionPort())
		{
			printf_s("[DEBUG] CreateIoCompletionPort error: %d\n", GetLastError());
			stateOpt = false;
			break;
		}

	} while (false);


	if (!stateOpt)
	{
		printf_s("[DEBUG][%s] CreateIoCompletionPort error: %d\n", __FUNCTION__, GetLastError());
		return stateOpt;
	}

	char clientIP[32] = { 0, };
	inet_ntop(AF_INET, &(mClientAddr.sin_addr), clientIP, 32 - 1);
	printf_s("[DEBUG] Client Connected: IP=%s, PORT=%d\n", clientIP, ntohs(mClientAddr.sin_port));

	if (false == PostRecv())
	{
		printf_s("[DEBUG][%s] PreRecv error: %d\n", __FUNCTION__, GetLastError());
		return false;
	}







	static int id = 101;
	++id;





	printf_s("[DEBUG][%s] Connectd New Session: %I64u\n", __FUNCTION__, mSocket);

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



bool Session::PostRecv()
{
	if (!IsConnected()) 
	{
		return false;
	}

	ExOverlappedIO* recvContext = new ExOverlappedIO(this, IOType::IO_RECV);

	DWORD recvbytes = 0;
	DWORD flags = 0;
	recvContext->mWsaBuf.len = BUFSIZE;
	
	memcpy(recvContext->mWsaBuf.buf, mRecvBuffer, sizeof(mRecvBuffer));


	printf_s("[DEBUG] PostRecv  ... . .. \n");




	return true;
}