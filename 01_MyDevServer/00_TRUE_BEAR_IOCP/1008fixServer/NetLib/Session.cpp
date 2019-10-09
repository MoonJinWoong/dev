#include "Session.h"

Session::Session(unsigned int UID)
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


	if (mAcceptEx == NULL)
	{
		DWORD bytes;
		// AcceptEx는 여타 소켓함수와 달리 직접 호출하는 것이 아니고,
		// 함수 포인터를 먼저 가져온 다음 호출할 수 있다. 그것을 여기서 한다.
		WSAIoctl(mSocket,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GUID(WSAID_ACCEPTEX),
			sizeof(GUID),
			&mAcceptEx,
			sizeof(mAcceptEx),
			&bytes,
			NULL,
			NULL);

		if (AcceptEx == NULL)
		{
			std::cout<<"Getting AcceptEx ptr failed." <<std::endl;
		}
	}


	// AcceptEx 
	CustomOverlapped* pOver = new CustomOverlapped(this,IOType::IO_ACCEPT);
	DWORD bytes = 0;
	DWORD flags = 0;
	pOver->mWsaBuf.len = 0;
	pOver->mWsaBuf.buf = nullptr;

	if (!AcceptEx(listenSocket, mSocket, addr, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytes, &pOver->mOverlapped))
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

void Session::AcceptFinish(SOCKET &listenSocket, HANDLE& hIocp)
{
	bool resultOk = true;
	do
	{
		if (SOCKET_ERROR == setsockopt(mSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&listenSocket, sizeof(SOCKET)))
		{
			printf_s("[DEBUG] SO_UPDATE_ACCEPT_CONTEXT error: %d\n", GetLastError());
			resultOk = false;
			break;
		}

		int opt = 1;
		if (SOCKET_ERROR == setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)& opt, sizeof(int)))
		{
			printf_s("[DEBUG] TCP_NODELAY error: %d\n", GetLastError());
			resultOk = false;
			break;
		}

		opt = 0;
		if (SOCKET_ERROR == setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (const char*)& opt, sizeof(int)))
		{
			printf_s("[DEBUG] SO_RCVBUF change error: %d\n", GetLastError());
			resultOk = false;
			break;
		}

		int addrlen = sizeof(SOCKADDR_IN);
		if (SOCKET_ERROR == getpeername(mSocket, (SOCKADDR*)& mClientAddr, &addrlen))
		{
			printf_s("[DEBUG] getpeername error: %d\n", GetLastError());
			resultOk = false;
			break;
		}

		/*HANDLE handle = CreateIoCompletionPort((HANDLE)mSocket,  &hIocp, (ULONG_PTR)this, 0);
		if (handle != hIocp)
		{
			printf_s("[DEBUG] CreateIoCompletionPort error: %d\n", GetLastError());
			resultOk = false;
			break;
		}*/

	} while (false);
	
}
