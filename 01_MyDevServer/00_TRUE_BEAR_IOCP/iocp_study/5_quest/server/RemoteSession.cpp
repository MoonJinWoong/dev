#include "RemoteSession.h"


RemoteSession::RemoteSession()
{
	ZeroMemory(&mRecvOver, sizeof(CustomOverEx));
	ZeroMemory(&mSendOver, sizeof(CustomOverEx));
	mRemoteSock = INVALID_SOCKET;
}


bool RemoteSession::SendPushInLogic(c_u_Int size, char* pMsg)
{
	// 무조건 로직스레드 에서만 콜해야 된다. 
	DWORD dwRecvNumBytes = 0;
	CopyMemory(mSendOver.m_SendBuf, pMsg, size);

	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	mSendOver.m_wsaBuf.len = size;
	mSendOver.m_wsaBuf.buf = mSendOver.m_SendBuf;
	mSendOver.m_eOperation = IOOperation::SEND;
	mSendOver.m_SendBuf[size] = NULL;



	// 여기서부터 다시.
	auto sendOver = new CustomOverEx;
	ZeroMemory(sendOver, sizeof(CustomOverEx));
	sendOver->m_wsaBuf.len = size;
	sendOver->m_wsaBuf.buf = new char[size];
	CopyMemory(sendOver->m_wsaBuf.buf, pMsg, size);
	sendOver->m_eOperation = IOOperation::SEND;


	std::lock_guard<std::mutex> guard(mSendLock);



}

void RemoteSession::SendPop(c_u_Int size)
{
	printf("[송신 완료] bytes : %d\n", size);

	auto_lock guard(mSendLock);

	delete[] mSendDataqueue.front()->m_wsaBuf.buf;
	delete mSendDataqueue.front();

	mSendDataqueue.pop();

	if (!mSendDataqueue.empty())
	{
		SendMsg();
	}
}

bool RemoteSession::SendMsg()
{
	int nRet = WSASend(mRemoteSock,
		&(mSendOver.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED) & (mSendOver),
		NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSASend()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

bool RemoteSession::RecvMsg()
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
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

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}


