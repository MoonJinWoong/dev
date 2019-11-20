#include "RemoteSession.h"


RemoteSession::RemoteSession()
{
	ZeroMemory(&mOverEx, sizeof(CustomOverEx));
	//ZeroMemory(&mSendOver, sizeof(CustomOverEx));
	mRemoteSock = INVALID_SOCKET;
}


//bool RemoteSession::SendPushInLogic(u_Int size, char* pMsg)
//{
//	// 무조건 로직스레드 에서만 콜해야 된다. 
//	auto sendOver = new CustomOverEx;
//	ZeroMemory(sendOver, sizeof(CustomOverEx));
//	sendOver->m_wsaBuf.len = size;
//	sendOver->m_wsaBuf.buf = new char[size];
//	CopyMemory(sendOver->m_wsaBuf.buf, pMsg, size);
//	sendOver->m_eOperation = IOOperation::SEND;
//
//	auto_lock guard(mSendLock);
//
//	mSendQ.push(sendOver);
//
//	if (mSendQ.size() == 1)
//	{
//		SendMsg();
//	}
//
//	return true;
//}
//
//void RemoteSession::SendPop()
//{
//	auto_lock guard(mSendLock);
//
//	delete[] mSendQ.front()->m_wsaBuf.buf;
//	delete mSendQ.front();
//
//	mSendQ.pop();
//
//	if (!mSendQ.empty())
//	{
//		SendMsg();
//	}
//}
//
//void RemoteSession::SendMsg()
//{
//
//	auto sendOver = mSendQ.front();
//	DWORD byte = 0;
//
//	int nRet = WSASend(mRemoteSock,
//		&(sendOver->m_wsaBuf),
//		1,
//		&byte,
//		0,
//		(LPWSAOVERLAPPED)sendOver,
//		NULL);
//	
//	
//	printf("[Send] Byte: %d \n", byte);
//
//	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
//	{
//		std::cout << " Err WSASend() code: " << WSAGetLastError() << std::endl;
//	}
//}

bool RemoteSession::RecvMsg()
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
	mOverEx.m_wsaBuf.len = MAX_SOCKBUF;
	mOverEx.m_wsaBuf.buf = mOverEx.m_RecvBuf;
	mOverEx.m_eOperation = IOOperation::RECV;

	int nRet = WSARecv(mRemoteSock,
		&(mOverEx.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (mOverEx),
		NULL);

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}


