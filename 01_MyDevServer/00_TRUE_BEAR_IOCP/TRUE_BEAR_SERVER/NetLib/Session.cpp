#include "Session.h"

Session::Session()
{

}

Session::~Session()
{

}
void Session::InnerInit()
{
	ZeroMemory(m_szIP, MAX_IP_LENGTH);

	m_RingRecvBuffer.Init();
	m_RingSendBuffer.Init();

	m_IsConnect = FALSE;
	m_IsClosed = FALSE;
	m_IsSendable = TRUE;

	m_SendIORefCount = 0;
	m_RecvIORefCount = 0;
	m_AcceptIORefCount = 0;
}
void Session::Init(SOCKET sock, int index)
{
	m_ListenSocket = sock;
	m_Index = index;
	m_RecvBufSize = MAX_RECV_OVERLAPPED_BUFF_SIZE;
	m_SendBufSize = MAX_SEND_OVERLAPPED_BUFF_SIZE;

	InnerInit();

	m_pRecvOverlappedEx = new CustomOverlapped(index);
	m_pSendOverlappedEx = new CustomOverlapped(index);
	m_RingRecvBuffer.Create(SESSION_MAX_RECV_BUFF_SIZE);
	m_RingSendBuffer.Create(SESSION_MAX_SEND_BUFF_SIZE);

	m_ConnectionMsg.Type = MsgType::Session;
	m_ConnectionMsg.pContents = nullptr;
	m_CloseMsg.Type = MsgType::Close;
	m_CloseMsg.pContents = nullptr;

}

void Session::DoAcceptOverlapped()
{
	ZeroMemory(&m_pRecvOverlappedEx->Overlapped, sizeof(OVERLAPPED));

	m_pRecvOverlappedEx->OverlappedExWsaBuf.buf = m_AddrBuf;
	m_pRecvOverlappedEx->pOverlappedExSocketMessage = m_pRecvOverlappedEx->OverlappedExWsaBuf.buf;
	m_pRecvOverlappedEx->OverlappedExWsaBuf.len = m_RecvBufSize;
	m_pRecvOverlappedEx->type = OPType::Accept;
	m_pRecvOverlappedEx->SessionIdx = GetIndex();

	m_ClientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_ClientSocket == INVALID_SOCKET)
	{
		std::cout << "WSASocket fail in DoAcceptOverlapped" << std::endl;
	}

	//IncrementAcceptIORefCount();

	DWORD acceptByte = 0;
	auto result = AcceptEx(
		m_ListenSocket,
		m_ClientSocket,
		m_pRecvOverlappedEx->OverlappedExWsaBuf.buf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&acceptByte,
		reinterpret_cast<LPOVERLAPPED>(m_pRecvOverlappedEx));

	if (!result && WSAGetLastError() != WSA_IO_PENDING)
	{
		// 에러 났으면 그냥 리슨 소켓을 닫는다.
		closesocket(m_ListenSocket);
		
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

bool Session::BindIOCP(const HANDLE hWorkIOCP)
{
	std::lock_guard<std::mutex> Lock(m_MUTEX);

	//즉시 접속 종료하기 위한 소켓 옵션 추가
	linger li = { 0, 0 };
	li.l_onoff = 1;
	setsockopt(m_ClientSocket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&li), sizeof(li));

	auto hIOCPHandle = CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(m_ClientSocket),
		hWorkIOCP,
		reinterpret_cast<ULONG_PTR>(this),
		0);

	if (hIOCPHandle == INVALID_HANDLE_VALUE || hWorkIOCP != hIOCPHandle)
	{
		return false;
	}

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

	auto moveMark = static_cast<int>(remainByte - (m_RingRecvBuffer.GetCurMark() - pNextBuf));
	m_pRecvOverlappedEx->OverlappedExWsaBuf.len = m_RecvBufSize;
	m_pRecvOverlappedEx->OverlappedExWsaBuf.buf = m_RingRecvBuffer.ForwardMark(moveMark, m_RecvBufSize, remainByte);

	if (m_pRecvOverlappedEx->OverlappedExWsaBuf.buf == nullptr)
	{
		return false;;
	}

	m_pRecvOverlappedEx->pOverlappedExSocketMessage = m_pRecvOverlappedEx->OverlappedExWsaBuf.buf - remainByte;

	ZeroMemory(&m_pRecvOverlappedEx->Overlapped, sizeof(OVERLAPPED));

	//IncrementRecvIORefCount();

	DWORD flag = 0;
	DWORD recvByte = 0;
	auto result = WSARecv(
		m_ClientSocket,
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

	std::lock_guard<std::mutex> Lock(m_MUTEX);

	if (m_IsConnect == TRUE)
	{
		m_IsConnect = FALSE;
	}

	if (m_ClientSocket != INVALID_SOCKET)
	{
		closesocket(m_ClientSocket);
		m_ClientSocket = INVALID_SOCKET;
	}
}