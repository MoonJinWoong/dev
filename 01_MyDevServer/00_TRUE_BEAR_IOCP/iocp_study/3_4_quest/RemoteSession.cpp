#include "RemoteSession.h"


RemoteSession::RemoteSession()
{
	ZeroMemory(&mRecvOver, sizeof(CustomOverEx));
	ZeroMemory(&mSendOver, sizeof(CustomOverEx));
	mRemoteSock = INVALID_SOCKET;
}


bool RemoteSession::SendMsg(const unsigned int size, char* pMsg)
{
	DWORD dwRecvNumBytes = 0;
	//���۵� �޼����� ����
	CopyMemory(mSendOver.m_SendBuf, pMsg, size);

	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	mSendOver.m_wsaBuf.len = size;
	mSendOver.m_wsaBuf.buf = mSendOver.m_SendBuf;
	mSendOver.m_eOperation = IOOperation::SEND;
	mSendOver.m_SendBuf[size] = NULL;

	int nRet = WSASend(mRemoteSock,
		&(mSendOver.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED) & (mSendOver),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSASend()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}
	return true;
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


