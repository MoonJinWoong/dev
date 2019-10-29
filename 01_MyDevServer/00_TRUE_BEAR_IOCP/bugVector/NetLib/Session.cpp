#include "Session.h"

Session::Session() 
{
	std::cout << "this construct not intended" << std::endl;
}
//Session::Session(SOCKET listenSock , int index) 
	//mListenSock(listenSock),					// listen socket set 
	//mIndex(index),								// unique id 
	//m_RecvBufSize(MAX_RECV_OVERLAPPED_BUFF_SIZE),
	//m_SendBufSize(MAX_SEND_OVERLAPPED_BUFF_SIZE),
	//m_IsConnect(false),
	//m_IsSendable(true),
	//m_SendIORefCount(0),
	//m_RecvIORefCount(0)
//{	
	//m_pRecvOverlappedEx = new CustomOverlapped(index);
	//m_pSendOverlappedEx = new CustomOverlapped(index);

	//m_ConnectionMsg.Type = MsgType::Session;
	//m_ConnectionMsg.pContents = nullptr;
	//m_CloseMsg.Type = MsgType::Close;
	//m_CloseMsg.pContents = nullptr;

	//m_CircleRecvBuffer.Create(SESSION_MAX_RECV_BUFF_SIZE);
	//m_CircleSendBuffer.Create(SESSION_MAX_SEND_BUFF_SIZE);
	//m_CircleRecvBuffer.Init();
	//m_CircleSendBuffer.Init();
//}

Session::~Session()
{

}


void Session::AcceptOverlapped()
{
	//TODO 끊어졌을때도 다시 걸어놔야 한다.
	// 끊어질때 시간 딜레이를 주는 방법을 생각해보자.
	m_pRecvOverlappedEx->OverlappedExWsaBuf.buf = m_AddrBuf;
	m_pRecvOverlappedEx->pOverlappedExSocketMessage = m_pRecvOverlappedEx->OverlappedExWsaBuf.buf;
	m_pRecvOverlappedEx->OverlappedExWsaBuf.len = m_RecvBufSize;
	m_pRecvOverlappedEx->type = OPType::Accept;



	// 2. create socket 
	mRemoteSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mRemoteSock == INVALID_SOCKET)
	{
		std::cout << "WSASocket fail in AcceptOverlapped" << std::endl;
	}

	//IncrementAcceptIORefCount();

	// 3. get function pointer 
	LPFN_ACCEPTEX pFnAcceptEx = (LPFN_ACCEPTEX)GetSockExAPI(WSAID_ACCEPTEX);


	// 4. call AcceptEx
	unsigned long acceptByte = 0;    // 0으로 설정 : 접속과 동시에 데이터를 수신하지 않겠다. 
	char szBuff[4096];
	ZeroMemory(&m_pRecvOverlappedEx->Overlapped, sizeof(OVERLAPPED));
	
	
	// 새로운 버젼
	auto result = pFnAcceptEx(
		mListenSock,
		mRemoteSock,
		szBuff,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&acceptByte,
		&m_pRecvOverlappedEx->Overlapped
	);


	// 기존꺼
	//auto result = AcceptEx(
	//	mListenSock,
	//	mRemoteSock,
	//	m_pRecvOverlappedEx->OverlappedExWsaBuf.buf,
	//	0,
	//	sizeof(SOCKADDR_IN) + 16,
	//	sizeof(SOCKADDR_IN) + 16,
	//	&acceptByte,
	//	reinterpret_cast<LPOVERLAPPED>(m_pRecvOverlappedEx)
	//);

	if (!result && WSAGetLastError() != WSA_IO_PENDING)
	{
		// 에러 났으면 그냥 리슨 소켓을 닫는다.
		closesocket(mListenSock);
		
		//DecrementAcceptIORefCount();
		std::cout << "AcceptEX fail" << std::endl;
	}
}

bool Session::GetLocalAndRemoteAddr()
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

bool Session::PostRecv(const char* pNextBuf, const unsigned long remainByte)
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
	if (m_pRecvOverlappedEx->OverlappedExWsaBuf.buf == nullptr)
	{
		//TODO 1029 오버 플로우 났을 경우인데 이거는 아예 잘못된 경우라서 해제 해주어야 
		// 할 것 같다.
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

	if (m_IsConnect == true)
	{
		m_IsConnect = false;
	}

	if (mRemoteSock != INVALID_SOCKET)
	{
		closesocket(mRemoteSock);
		mRemoteSock = INVALID_SOCKET;
	}
}

void* Session::GetSockExAPI(GUID guidFn)
{
	// 내가 얻고 싶은 소켓 확장 api 얻어옴.
	void* pFnEx = NULL;
	GUID guid = guidFn;
	unsigned long dwBytes = 0;

	auto ret = WSAIoctl(
		mListenSock,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid, sizeof(guid),
		&pFnEx, sizeof(pFnEx),
		&dwBytes, NULL, NULL
	);

	if (ret == SOCKET_ERROR)
	{
		return NULL;
	}

	return pFnEx;



}

