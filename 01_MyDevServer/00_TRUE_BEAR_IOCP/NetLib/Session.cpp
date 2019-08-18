#include "Session.h"

void IoData::Init()
{
	ZeroMemory(&overlapped, sizeof(overlapped));
	ioType = IO_ERROR;

}

void IoData::Clear()
{
	msgBuffer.fill(0);
	totalByte = 0;
	currByte  = 0;
}

void IoData::setType(IO_TYPE iotype)
{
	ioType = iotype;
}

bool IoData::isCompleteRecv(int size)
{
	currByte += size;
	if (currByte >= totalByte)
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////

Session::Session()
{
	this->Init();
}

void Session::Init()
{
	ZeroMemory(&mSockData, sizeof(mSockData));
	mIoData[IO_READ].setType(IO_READ);
	mIoData[IO_WRITE].setType(IO_WRITE);
}

void Session::isError(DWORD ret)
{
	if (ret == SOCKET_ERROR
		&& WSAGetLastError() != ERROR_IO_PENDING)
	{
		cout << "socket error : " << WSAGetLastError() << endl;
	}
}

void Session::recv(WSABUF wsabuf)
{
	DWORD flag = 0;
	DWORD recvByte = 0;
	
	auto ret = WSARecv(mSockData.sSocket, &wsabuf, 1, &recvByte, &flag,
		(LPWSAOVERLAPPED)&(mIoData[IO_READ].overlapped), NULL);

	this->isError(ret);
}

bool Session::checkReceive(int size)
{
	if (mIoData[IO_READ].isCompleteRecv(size))
	{
		this->recv(mIoData[IO_READ].dataBuf);
		return true;
	}
	return false;
}

void Session::recvReady()
{
	mIoData[IO_READ].Clear();

	WSABUF wsabuf;
	wsabuf.buf = mIoData[IO_READ].msgBuffer.data();
	wsabuf.len = MAX_BUFFER;

	this->recv(wsabuf);
}





void Session::send(WSABUF wsabuf)
{
	DWORD flag = 0;
	DWORD sendByte = 0;

	auto ret = WSASend(mSockData.sSocket, &wsabuf, 1, &sendByte, flag,
		(LPWSAOVERLAPPED) & (mIoData[IO_WRITE].overlapped), NULL);

	this->isError(ret);
}

void Session::checkSend(int size)
{
	if (mIoData[IO_READ].isCompleteRecv(size))
	{
		this->send(mIoData[IO_WRITE].dataBuf);
	}
}

void Session::sendMsg()
{
	// 패키지 파라미터로 받아야함.
}


void Session::onClose()
{
	// 여기 작업해야함
}

void Session::setID(int id)
{
	m_ID = id;
}
