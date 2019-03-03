#include "preCompile.h"



ConnectionManager::ConnectionManager()
{
	cout << "connection manager ������ ȣ�� �Ǵ� ...." << endl;
	InitializeConnection();
}

ConnectionManager::~ConnectionManager()
{

}

void ConnectionManager::InitializeConnection()
{
	ZeroMemory(cIpAddress, MAX_IP_LENGTH);
	cSocket = INVALID_SOCKET;
	cIsConnect = FALSE;
	cIsClosed = FALSE;
	cIsSend = TRUE;
	//m_dwSendIoRefCount = 0;
	//m_dwRecvIoRefCount = 0;
	//m_dwAcceptIoRefCount = 0;

	cRecvCircleBuffer.InitializeCircleBuffer();
	cSendCircleBuffer.InitializeCircleBuffer();
}

bool ConnectionManager::CreateConnection(INITCONFIG &InitConfig)
{
	cIndex = InitConfig.sIndex;
	cListenSocket = InitConfig.sListenSocket;

	cRecvOverlappedEx = new OVERLAPPED_EX(this);
	cSendOverlappedEx = new OVERLAPPED_EX(this);
	cRecvCircleBuffer.CreateCircleBuffer(InitConfig.sRecvBufSize * InitConfig.sRecvBufCnt);
	cSendCircleBuffer.CreateCircleBuffer(InitConfig.sSendBufSize * InitConfig.sSendBufCnt);


	cRecvBufSize = InitConfig.sRecvBufSize;
	cSendBufSize = InitConfig.sSendBufSize;

	return BindAcceptExSock();
}

bool ConnectionManager::BindAcceptExSock()
{
	//���� ������ ���ٸ� acceptex�� bind���� �ʴ´�.
	if (0 == cListenSocket)
		return true;


	DWORD dwBytes = 0;
	memset(&cRecvOverlappedEx->sOverlapped, 0, sizeof(OVERLAPPED));
	cRecvOverlappedEx->sWsaBuf.buf = cAddressBuf;
	cRecvOverlappedEx->slpSocketMsg = &cRecvOverlappedEx->sWsaBuf.buf[0];
	cRecvOverlappedEx->sWsaBuf.len = cRecvBufSize;
	cRecvOverlappedEx->sOperation = OP_ACCEPT;
	cRecvOverlappedEx->slpConnection = this;


	cSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == cSocket)
	{
		cout << "ConnectionManager-> BindAcceptExSock()--> socket ����� failed....!!" << endl;
		return false;
	}

	//IncrementAcceptIoRefCount();

	BOOL bRet = AcceptEx(cListenSocket, cSocket,
		cRecvOverlappedEx->sWsaBuf.buf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&dwBytes,
		(LPOVERLAPPED)cRecvOverlappedEx
	);


	if (!bRet && WSAGetLastError() != WSA_IO_PENDING)
	{
		//DecrementAcceptIoRefCount();
		cout << "BindAcceptExSock() failed...!" << endl;
		return false;
	}
	return true;
}
bool ConnectionManager::CloseConnection(bool bForce)
{
	// ���ɾ����

		struct linger li = { 0, 0 };	// Default: SO_DONTLINGER
		if (bForce)
			li.l_onoff = 1; // SO_LINGER, timeout = 0

	//	if (NULL != IocpServer() && cIsConnect == true)
			//IocpServer()->OnClose(this);

		shutdown(cSocket, SD_BOTH);
		setsockopt(cSocket, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li));
		closesocket(cSocket);

		cSocket = INVALID_SOCKET;
		if (cRecvOverlappedEx != NULL)
		{
			cRecvOverlappedEx->sDwRemain = 0;
			cRecvOverlappedEx->sTotalBytes = 0;
		}

		if (cSendOverlappedEx != NULL)
		{
			cSendOverlappedEx->sDwRemain = 0;
			cSendOverlappedEx->sTotalBytes = 0;
		}


		//connection�� �ٽ� �ʱ�ȭ �����ش�.
		InitializeConnection();
		BindAcceptExSock();
	
	return true;
}


char* ConnectionManager::PrepareSendPacket(int len)
{
	// broad cast ���� �Լ� 
	if (false == cIsConnect)
		return NULL;

	char* pBuf = cSendCircleBuffer.ForwardMark(len);

	if (NULL == pBuf)
	{
		//IocpServer()->CloseConnection(this);
		cout << "PrepareSendPacket() call failed.....!!" << endl;
		return NULL;
	}


	ZeroMemory(pBuf, len);
	CopyMemory(pBuf, &len, PACKET_SIZE_LENGTH);
	return pBuf;
}

bool ConnectionManager::SendPost(int SendSize)
{
	DWORD dwBytes;
	//������ ���� �ִٸ�, �� IocpServer class�� DoSend()���� �Ҹ��� �ƴ϶�
	// PrepareSendPacket()�Լ��� �θ��� 
	//SendPost�� �ҷȴٸ� ������ ���� �ְ� DoSend���� �ҷȴٸ� 0�� �´�.

	if (SendSize > 0)
		cSendCircleBuffer.SetUsedBufferSize(SendSize);

	
	//�� �ɾ����
		int ReadSize;
		char* pBuf = cRecvCircleBuffer.GetBuffer(cSendBufSize, &ReadSize);
		if (NULL == pBuf)
		{
			cout << "CircleBuffer GetBuffer() call failed...!!" << endl;
			return false;
		}

		// overlapped setting

		cSendOverlappedEx->sDwRemain = 0;
		cSendOverlappedEx->sOperation = OP_SEND;
		cSendOverlappedEx->sTotalBytes = ReadSize;
		ZeroMemory(&cSendOverlappedEx->sOverlapped, sizeof(OVERLAPPED));
		cSendOverlappedEx->sWsaBuf.len = ReadSize;
		cSendOverlappedEx->sWsaBuf.buf = pBuf;
		cSendOverlappedEx->slpConnection = this;

		// ��
		//IncrementSendIoRefCount();




		// ��¥ send
		int ret = WSASend(
			cSocket,
			&cSendOverlappedEx->sWsaBuf,
			1,
			&dwBytes,
			0,
			&cSendOverlappedEx->sOverlapped,
			NULL);
		if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			//DecrementSendIoRefCount();
			//IocpServer()->CloseConnection(this);
			return false;
		}
	
		//�� Ǯ�����
	return true;
}

