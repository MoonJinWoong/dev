#include "RemoteSession.h"
#pragma comment(lib,"mswsock.lib")


RemoteSession::RemoteSession()
{
	ZeroMemory(&mOverIo, sizeof(CustomOverEx));
	mRemoteSock = INVALID_SOCKET;
}


bool RemoteSession::SendPushInLogic(c_u_Int size, char* pMsg)
{
	// ������ ���������� ������ ���ؾ� �ȴ�. 
	auto sendOver = new CustomOverEx;
	ZeroMemory(sendOver, sizeof(CustomOverEx));
	sendOver->m_wsaBuf.len = size;
	sendOver->m_wsaBuf.buf = new char[size];
	CopyMemory(sendOver->m_wsaBuf.buf, pMsg, size);
	sendOver->m_eOperation = IOOperation::SEND;

	auto_lock guard(mSendLock);

	mSendQ.push(sendOver);

	if (mSendQ.size() == 1)
	{
		SendMsg();
	}

	return true;
}

void RemoteSession::SendPopInWorker()
{
	auto_lock guard(mSendLock);

	delete[] mSendQ.front()->m_wsaBuf.buf;
	delete mSendQ.front();

	mSendQ.pop();

	if (!mSendQ.empty())
	{
		SendMsg();
	}
}

void RemoteSession::SendMsg()
{
	auto sendOver = mSendQ.front();
	DWORD byte = 0;

	int nRet = WSASend(mRemoteSock,
		&(sendOver->m_wsaBuf),
		1,
		&byte,
		0,
		(LPWSAOVERLAPPED)sendOver,
		NULL);
	
	
	printf("[Send] Byte: %d \n", byte);

	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		std::cout << " Err WSASend() code: " << WSAGetLastError() << std::endl;
	}
}


bool RemoteSession::AccpetAsync(SOCKET listenSock)
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

	if(!ret)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf_s("AcceptEx Error : %d\n", GetLastError());
			return false;
		}
	}

	return true;

}

bool RemoteSession::AcceptFinish(HANDLE iocpHandle)
{
	mIsConnected = true;

	SOCKADDR_IN		RemoteAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);
	char clientIP[32] = { 0, };
	inet_ntop(AF_INET, &(RemoteAddr.sin_addr), clientIP, 32 - 1);
	printf("���� : INDEX(%d) , SOCKET(%d)\n", (int)mIdx,mRemoteSock);

	RecvStart();
	return true;
}


bool RemoteSession::RecvZeroByte()
{
	// ������ �ޱ������� page locking ���� �ʰڴ�.
    // zero byte recv

	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;
	mOverIo.m_wsaBuf.len = 0;
	mOverIo.m_wsaBuf.buf = nullptr;
	mOverIo.m_eOperation = IOOperation::RECV;

	int nRet = WSARecv(mRemoteSock,
		&(mOverIo.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (mOverIo),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}
	return true;
}
bool RemoteSession::RecvStart()
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	mOverIo.m_wsaBuf.len = MAX_SOCKBUF;
	mOverIo.m_wsaBuf.buf = mOverIo.m_RecvBuf;
	mOverIo.m_eOperation = IOOperation::RECV;

	int nRet = WSARecv(mRemoteSock,
		&(mOverIo.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (mOverIo),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

void RemoteSession::Release(bool IsForce , SOCKET listenSock)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

	// ���� ����
	if (IsForce)
	{
		stLinger.l_onoff = 1;
	}

	printf("���� : INDEX(%d) , SOCKET(%d)\n", (int)mIdx, mRemoteSock);
	
	
	shutdown(mRemoteSock, SD_BOTH);
	setsockopt(mRemoteSock, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));
	closesocket(mRemoteSock);

	mRemoteSock = INVALID_SOCKET;
	mIsConnected = false;



	// �񵿱� Accept �ٽ� �ɾ��༭ ����
	AccpetAsync(listenSock);
}

