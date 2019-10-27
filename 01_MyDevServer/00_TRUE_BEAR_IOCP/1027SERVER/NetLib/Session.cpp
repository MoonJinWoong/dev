#include "Session.h"

Session::Session()
{
	std::cout << "this construct not intended" << std::endl;
}
Session::Session(SOCKET listenSock , int index) :
		mListenSock(listenSock),					// listen socket set 
	    mIndex(index),								// unique id 
		m_RecvBufSize(MAX_RECV_OVERLAPPED_BUFF_SIZE),
		m_SendBufSize(MAX_SEND_OVERLAPPED_BUFF_SIZE),
		m_IsConnect(false),
		m_IsClosed(false),
		m_IsSendable(true),
		m_SendIORefCount(0),
		m_RecvIORefCount(0),
		m_AcceptIORefCount(0)
{
	//ZeroMemory(m_szIP, MAX_IP_LENGTH);
	
	m_pRecvOverlappedEx = new CustomOverlapped(index);
	m_pSendOverlappedEx = new CustomOverlapped(index);

	m_ConnectionMsg.Type = MsgType::Session;
	m_ConnectionMsg.pContents = nullptr;
	m_CloseMsg.Type = MsgType::Close;
	m_CloseMsg.pContents = nullptr;


	m_CircleRecvBuffer.Create(SESSION_MAX_RECV_BUFF_SIZE);
	m_CircleSendBuffer.Create(SESSION_MAX_SEND_BUFF_SIZE);
	m_CircleRecvBuffer.Init();
	m_CircleSendBuffer.Init();
}

Session::~Session()
{

}


void Session::AcceptOverlapped()
{
	//TODO 끊어졌을때도 다시 걸어놔야 한다.
	// 끊어질때 시간 딜레이를 주는 방법을 생각해보자.
	ZeroMemory(&m_pRecvOverlappedEx->Overlapped, sizeof(OVERLAPPED));

	m_pRecvOverlappedEx->OverlappedExWsaBuf.buf = m_AddrBuf;
	m_pRecvOverlappedEx->pOverlappedExSocketMessage = m_pRecvOverlappedEx->OverlappedExWsaBuf.buf;
	m_pRecvOverlappedEx->OverlappedExWsaBuf.len = m_RecvBufSize;
	m_pRecvOverlappedEx->type = OPType::Accept;
	m_pRecvOverlappedEx->SessionIdx = GetIndex();

	mRemoteSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mRemoteSock == INVALID_SOCKET)
	{
		std::cout << "WSASocket fail in AcceptOverlapped" << std::endl;
	}

	//IncrementAcceptIORefCount();

	unsigned long acceptByte = 0;
	auto result = AcceptEx(
		mListenSock,
		mRemoteSock,
		m_pRecvOverlappedEx->OverlappedExWsaBuf.buf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&acceptByte,
		reinterpret_cast<LPOVERLAPPED>(m_pRecvOverlappedEx));

	if (!result && WSAGetLastError() != WSA_IO_PENDING)
	{
		// 에러 났으면 그냥 리슨 소켓을 닫는다.
		closesocket(mListenSock);
		
		//DecrementAcceptIORefCount();
		std::cout << "AcceptEX fail" << std::endl;
	}
}

bool Session::SetNetAddressInfo()
{
	SOCKADDR* pLocalSockAddr = nullptr;
	SOCKADDR* pRemoteSockAddr = nullptr;

	int	localSockaddrLen = 0;
	int	remoteSockaddrLen = 0;

	GetAcceptExSockaddrs(m_AddrBuf, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		&pLocalSockAddr, &localSockaddrLen,
		&pRemoteSockAddr, &remoteSockaddrLen);

	if (remoteSockaddrLen != 0)
	{
		SOCKADDR_IN* pRemoteSockAddrIn = reinterpret_cast<SOCKADDR_IN*>(pRemoteSockAddr);
		if (pRemoteSockAddrIn != nullptr)
		{
			char szIP[MAX_IP_LENGTH] = { 0, };
			inet_ntop(AF_INET, &pRemoteSockAddrIn->sin_addr, szIP, sizeof(szIP));
			SetRemoteIP(szIP);
		}
		return true;
	}
	return false;
}

bool Session::CloseComplete()
{
	// 구현해야함.

	return true;
}

void Session::UpdateSessionState()
{
	InterlockedExchange(reinterpret_cast<LPLONG>(&m_IsConnect), TRUE);
}

bool Session::PostRecv(const char* pNextBuf, const DWORD remainByte)
{
	if (m_IsConnect == FALSE || m_pRecvOverlappedEx == nullptr)
	{
		return false;
	}

	m_pRecvOverlappedEx->type = OPType::Recv;
	m_pRecvOverlappedEx->OverlappedExRemainByte = remainByte;

	auto moveMark = static_cast<int>(remainByte - (m_CircleRecvBuffer.GetCurMark() - pNextBuf));
	m_pRecvOverlappedEx->OverlappedExWsaBuf.len = m_RecvBufSize;
	m_pRecvOverlappedEx->OverlappedExWsaBuf.buf = m_CircleRecvBuffer.ForwardMark(moveMark, m_RecvBufSize, remainByte);
	//TODO 포워드 마크에서 반환값이 널이 나오는 경우가 있는지 다시 확인해볼 것
	// 
	if (m_pRecvOverlappedEx->OverlappedExWsaBuf.buf == nullptr)
	{
		//TODO 반환값을 실패했을때 처리를 잘하고 있는지 다시 확인해볼 것
		return false;;
	}

	m_pRecvOverlappedEx->pOverlappedExSocketMessage = m_pRecvOverlappedEx->OverlappedExWsaBuf.buf - remainByte;

	ZeroMemory(&m_pRecvOverlappedEx->Overlapped, sizeof(OVERLAPPED));

	IncrementRecvIORefCount();

	unsigned long flag = 0;
	unsigned long recvByte = 0;
	auto result = WSARecv(
		mRemoteSock,
		&m_pRecvOverlappedEx->OverlappedExWsaBuf,
		1,
		&recvByte,
		&flag,
		&m_pRecvOverlappedEx->Overlapped,
		NULL);

	if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		// 유저가 접속을 끊었다.
		//DecrementRecvIORefCount();
		return false;
	}

	return true;
}

void Session::DisconnectSession()
{
	//TODO 유니크하게 호출되기 때문에. 락 안걸어도 된다.
	//std::lock_guard<std::mutex> Lock(m_MUTEX);

	if (m_IsConnect == TRUE)
	{
		m_IsConnect = FALSE;
	}

	if (mRemoteSock != INVALID_SOCKET)
	{
		closesocket(mRemoteSock);
		mRemoteSock = INVALID_SOCKET;
	}
}

