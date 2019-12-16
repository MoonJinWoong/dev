#include "RemoteSession.h"
#include "Packet.h"
#pragma comment(lib,"mswsock.lib")

RemoteSession::RemoteSession() : mRecvBuffer(1024),mSendBuffer(1024) // 옵션으로 받을 것
{
	ZeroMemory(&mRecvOverEx, sizeof(CustomOverEx));
	ZeroMemory(&mSendOverEx, sizeof(CustomOverEx));
	ZeroMemory(&mAcceptOverEx, sizeof(CustomOverEx));
	mRemoteSock = INVALID_SOCKET;
}

bool RemoteSession::AcceptReady(SOCKET listenSock)
{
	mRemoteSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mRemoteSock == INVALID_SOCKET)
	{
		std::cout << "[ERR] AccpetAsync WSASocket fail" << std::endl;
	}

	ZeroMemory(&mAcceptOverEx, sizeof(CustomOverEx));

	DWORD bytes = 0;
	DWORD flags = 0;
	mAcceptOverEx.mWSABuf.len = 0;
	mAcceptOverEx.mWSABuf.buf = nullptr;
	mAcceptOverEx.mIoType = IO_TYPE::ACCEPT;
	mAcceptOverEx.mUid = mUID;

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
			return false;
			printf_s("AcceptEx Error : %d\n", GetLastError());
		}
	}
	return true;
}
bool RemoteSession::AcceptFinish(HANDLE mIocp)
{
	SetIsLive();

	SOCKADDR_IN		RemoteAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);
	char clientIP[32] = { 0, };
	inet_ntop(AF_INET, &(RemoteAddr.sin_addr), clientIP, 32 - 1);

	std::cout << "[접속] index :"<<mUID<<"   socket num:"<<mRemoteSock << std::endl;

	RecvMsg();
	return true;
}

bool RemoteSession::SendReady(const unsigned int size, char* pData)
{
	mSendBuffer.SetWriteBuffer(pData, size);
	return true;
}


bool RemoteSession::SendPacket()
{
	if (mSendBuffer.GetReadAbleSize() == 0)
	{
		return false;
	}

	mSendOverEx.mWSABuf.len = mSendBuffer.GetReadAbleSize();
	mSendOverEx.mWSABuf.buf = mSendBuffer.GetReadBufferPtr();
	mSendOverEx.mIoType = IO_TYPE::SEND;

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
		std::cout << "[에러] WSASend() fail :" << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}

void RemoteSession::SendFinish(unsigned long len)
{
	//TODO 내가 send 요청한 사이즈랑 완료된 사이즈랑 다를때 처리해야함
	std::cout << "Send Complete byte:" << len << std::endl;	
	mSendBuffer.MoveReadPos(len);
}

bool RemoteSession::RecvMsg()
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;
	short count = 1;
	WSABUF wBuf[2];

	wBuf[0].buf = mRecvBuffer.GetWriteBuffer();
	wBuf[0].len = mRecvBuffer.GetWriteAbleSize();

	if (mRecvBuffer.GetRemainSize() > mRecvBuffer.GetWriteAbleSize())
	{
		wBuf[1].buf = mRecvBuffer.GetBufferPtr();
		wBuf[1].len = mRecvBuffer.GetRemainSize() - mRecvBuffer.GetWriteAbleSize();
		count++;
	}

	mRecvOverEx.mIoType = IO_TYPE::RECV;
	mRecvOverEx.mUid = mUID;

	auto ret = WSARecv(
		mRemoteSock,
		wBuf,
		count,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (mRecvOverEx),
		NULL
	);

	if (ret == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		std::cout << "[에러] WSARecv fail.. :" << WSAGetLastError()
			<< "   socket num:" << mRemoteSock << std::endl;
		return false;
	}
	return true;
}

void RemoteSession::RecvFinish(unsigned short size)
{
	mRecvBuffer.MoveReadPos(size);
}

bool RemoteSession::UnInit(bool IsForce, SOCKET mListenSock)
{
	//TODO IO가 남아있는 경우도 있을거고 
	// 유니크하게 실행될 수 있게 구현하자
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

	// 강제 종료
	if (IsForce)
	{
		stLinger.l_onoff = 1;
	}
	std::cout << "[Session] out -> " << mUID <<"   SOCKET:"<<mRemoteSock<< std::endl;


	shutdown(mRemoteSock, SD_BOTH);
	setsockopt(mRemoteSock, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
	closesocket(mRemoteSock);

	mRemoteSock = INVALID_SOCKET;
	mIsLive = false;

	mRecvBuffer.Init();
	mSendBuffer.Init();


	// accept 다시 걸어주자
	AcceptReady(mListenSock);
	return true;
}



