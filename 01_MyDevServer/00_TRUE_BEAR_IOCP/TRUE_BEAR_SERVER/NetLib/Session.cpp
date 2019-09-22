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

	IncrementAcceptIORefCount();

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
		DecrementAcceptIORefCount();

		std::cout << "AcceptEX fail" << std::endl;
	}
}