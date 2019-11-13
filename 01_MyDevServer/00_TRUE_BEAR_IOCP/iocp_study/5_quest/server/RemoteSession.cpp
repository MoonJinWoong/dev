#include "RemoteSession.h"


RemoteSession::RemoteSession()
{
	ZeroMemory(&mRecvOver, sizeof(CustomOverEx));
	//ZeroMemory(&mSendOver, sizeof(CustomOverEx));
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

void RemoteSession::SendPop()
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

bool RemoteSession::RecvMsg()
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	mRecvOver.m_wsaBuf.len = MAX_SOCKBUF;
	mRecvOver.m_wsaBuf.buf = mRecvOver.m_RecvBuf;
	mRecvOver.m_eOperation = IOOperation::RECV;

	int nRet = WSARecv(mRemoteSock,
		&(mRecvOver.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (mRecvOver),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}


