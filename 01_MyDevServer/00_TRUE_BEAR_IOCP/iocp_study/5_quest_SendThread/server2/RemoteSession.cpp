#include "RemoteSession.h"
#include "Packet.h"

RemoteSession::RemoteSession()
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

	ZeroMemory(&mOverAccept, sizeof(CustomOverEx));

	DWORD bytes = 0;
	DWORD flags = 0;
	mOverAccept.m_wsaBuf.len = 0;
	mOverAccept.m_wsaBuf.buf = nullptr;
	mOverAccept.m_eOperation = IOOperation::ACCEPT;
	mOverAccept.mUid = mIdx;

	auto ret = AcceptEx(
		listenSock,
		mRemoteSock,
		mAcceptBuf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&bytes,
		(LPWSAOVERLAPPED) & (mOverAccept)
	);

	if (!ret)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("AcceptEx Error : %d\n", GetLastError());
			return false;
		}
	}

	return true;
}
bool RemoteSession::AcceptFinish(HANDLE mhIocp)
{
	mIsLive = true;

	SOCKADDR_IN		RemoteAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);
	char clientIP[32] = { 0, };
	inet_ntop(AF_INET, &(RemoteAddr.sin_addr), clientIP, 32 - 1);
	printf("���� : INDEX(%d) , SOCKET(%d)\n", (int)mIdx, mRemoteSock);

	RecvMsg();
	return true;
}


bool RemoteSession::SendReady()
{
	return true;
}
bool RemoteSession::SendPacket(char* pBuf , int len)
{
	std::lock_guard<std::mutex> guard(mSendLock);

	/// ���� send�� �Ϸ�Ǿ�� �� �Լ����� ��Ŷ�� ������.
	if (mSendPendingCnt > 0)
		return false;

	// WSASend
	CustomOverEx* sendOver = new CustomOverEx;
	sendOver->mWSABuf.len = len;
	sendOver->mWSABuf.buf = pBuf;
	sendOver->mIoType = IOOperation::SEND;


	DWORD sendbytes = 0;
	DWORD flags = 0;
	auto ret = WSASend(
		mRemoteSock, 
		&sendOver->mWSABuf,
		1, 
		&sendbytes, 
		flags, 
		(LPWSAOVERLAPPED)sendOver,
		NULL);

	if (ret == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			// ���� �����ش�.
			delete sendOver;
			return true;
		}
	}

	if (sendbytes < len)
	{
		mSendOverEx.mRemainByte += len - sendbytes;
		return false;
	}
	
	++mSendPendingCnt;

	if (mSendPendingCnt == 1) 
		return true;
	else 
		return false;
}
void RemoteSession::SendFinish(unsigned long len)
{
	// IO �Ϸ� ���� �� ȣ�� �ȴ�.
	std::lock_guard<std::mutex> guard(mSendLock);
	--mSendPendingCnt;

	std::cout << "Send Complete byte:" << len << std::endl;
}

bool RemoteSession::RecvMsg()
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	mRecvOverEx.mWSABuf.len = MAX_SOCKBUF;
	mRecvOverEx.mWSABuf.buf = mRecvOverEx.mBuf.data();
	mRecvOverEx.mIoType = IOOperation::RECV;

	int nRet = WSARecv(mRemoteSock,
		&(mRecvOverEx.mWSABuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (mRecvOverEx),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

bool RemoteSession::RecvReady()
{
	// zero byte recv
	return true;
}

bool RemoteSession::RecvFinish(const char* pNext, const unsigned long remain)
{
	return true;
}

void RemoteSession::UnInit(bool IsForce, SOCKET listenSock)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

	// ���� ����
	if (IsForce)
	{
		stLinger.l_onoff = 1;
	}

	printf("���� : INDEX(%d) , SOCKET(%d)\n", (int)mUID, mRemoteSock);


	shutdown(mRemoteSock, SD_BOTH);
	setsockopt(mRemoteSock, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
	closesocket(mRemoteSock);

	mRemoteSock = INVALID_SOCKET;
	mIsLive = false;

	// �񵿱� Accept �ٽ� �ɾ��༭ ����
	AcceptReady(listenSock);
}


