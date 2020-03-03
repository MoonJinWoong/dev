#include "RemoteSession.h"
#include "Packet.h"
#include "easylogging++.h"
#pragma comment(lib,"mswsock.lib")


RemoteSession::RemoteSession() : mRecvBuffer(RECV_BUFFER_MAX_SIZE),mSendBuffer(SEND_BUFFER_MAX_SIZE)
{
	ZeroMemory(&mRecvOverEx, sizeof(CustomOverEx));
	ZeroMemory(&mSendOverEx, sizeof(CustomOverEx));
	ZeroMemory(&mAcceptOverEx, sizeof(CustomOverEx));
	mRemoteSock = INVALID_SOCKET;
}

void RemoteSession::Init()
{
	ZeroMemory(&mRecvOverEx, sizeof(CustomOverEx));
	ZeroMemory(&mSendOverEx, sizeof(CustomOverEx));
	ZeroMemory(&mAcceptOverEx, sizeof(CustomOverEx));

	mRecvBuffer.Init();
	mSendBuffer.Init();

	mIsLive.store(false);

	mIoRef.Init();
}
bool RemoteSession::AcceptReady(SOCKET listenSock)
{
	mRemoteSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mRemoteSock == INVALID_SOCKET)
	{
		LOG(ERROR) << "RemoteSession::WSASocket";
 	}

	ZeroMemory(&mAcceptOverEx, sizeof(CustomOverEx));

	DWORD bytes = 0;
	DWORD flags = 0;
	mAcceptOverEx.mWSABuf.len = 0;
	mAcceptOverEx.mWSABuf.buf = nullptr;
	mAcceptOverEx.mIoType = IO_TYPE::ACCEPT;
	mAcceptOverEx.mUid = mUID;

	mIoRef.IncAcptCount();

	auto ret = AcceptEx(
		listenSock,
		mRemoteSock,
		mAcceptBuf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&bytes,
		(LPWSAOVERLAPPED) & (mAcceptOverEx)
	);

	if (!ret)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			mIoRef.DecAcptCount();
			LOG(ERROR) << "RemoteSession::WSASocket : " << GetLastError();
			return false;
		}
	}
	return true;
}
bool RemoteSession::AcceptFinish(HANDLE mIocp)
{

	mIsLive.store(true);

	SOCKADDR_IN		RemoteAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);
	char clientIP[32] = { 0, };
	inet_ntop(AF_INET, &(RemoteAddr.sin_addr), clientIP, 32 - 1);

	LOG(INFO) << "[SUCCESS] Connect :  " << mUID;
	mIoRef.DecAcptCount();

	RecvIo(0);
	return true;
}

bool RemoteSession::SendReady(const unsigned int size, char* pData)
{
	mSendBuffer.SetWriteBuffer(pData, size);
	return true;
}


bool RemoteSession::SendIo()
{
	if (mSendBuffer.GetReadAbleSize() == 0)
	{
		return false;
	}

	mSendOverEx.mWSABuf.len = mSendBuffer.GetReadAbleSize();
	mSendOverEx.mWSABuf.buf = mSendBuffer.GetReadBufferPtr();
	mSendOverEx.mIoType = IO_TYPE::SEND;


	mIoRef.IncSendCount();
	DWORD dwRecvNumBytes = 0;
	int nRet = WSASend(mRemoteSock,
		&(mSendOverEx.mWSABuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED) & (mSendOverEx),
		NULL);

	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		LOG(ERROR) << "RemoteSession::WSASend : " << WSAGetLastError();
		mIoRef.DecSendCount();
		return false;
	}
	return true;
}

void RemoteSession::SendFinish(unsigned long len)
{
	mSendBuffer.MoveReadPos(len);
	mIoRef.DecSendCount();
}

bool RemoteSession::RecvIo(int remainSize)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;
	WSABUF wBuf;

	if (remainSize > 0)
	{
		mRecvBuffer.CheckWrite(remainSize);
	}

	wBuf.buf = mRecvBuffer.GetWriteBufferPtr();
	wBuf.len = RECV_BUFFER_MAX_SIZE;

	mRecvOverEx.mIoType = IO_TYPE::RECV;
	mRecvOverEx.mUid = mUID;

	mIoRef.IncRecvCount();

	auto ret = WSARecv(
		mRemoteSock,
		&wBuf,
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (mRecvOverEx),
		NULL
	);

	if (ret == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		LOG(ERROR) << "RemoteSession::WSARecv : " << WSAGetLastError();
		mIoRef.DecRecvCount();
		return false;
	}
	return true;
}

void RemoteSession::RecvFinish(unsigned short size)
{
	mRecvBuffer.MoveReadPos(size);
	mIoRef.DecRecvCount();
}

bool RemoteSession::DisconnectFinish(SOCKET mListenSock)
{
	if (mIoRef.GetRecvIoCount() != 0 || mIoRef.GetSendIoCount() != 0 || mIoRef.GetAcptIoCount() != 0)
	{
		return false;
	}
	
	if (mIsLive.load())
	{
		CloseSocket();
		return true;
	}
	return false;
}

bool RemoteSession::CloseSocket()
{
	mIsLive.store(false);

	shutdown(mRemoteSock, SD_BOTH);
	closesocket(mRemoteSock);
	mRemoteSock = INVALID_SOCKET;
	return true;
}



