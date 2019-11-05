
#include "NetService.h"

bool NetService::InitSocket()
{
	WSADATA wsaData;

	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		printf("[����] WSAStartup()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	//���������� TCP , Overlapped I/O ������ ����
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mListenSocket)
	{
		printf("[����] socket()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	printf("���� �ʱ�ȭ ����\n");
	return true;
}

bool NetService::BindandListen(int nBindPort)
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(nBindPort); //���� ��Ʈ�� �����Ѵ�.		
	//� �ּҿ��� ������ �����̶� �޾Ƶ��̰ڴ�.
	//���� ������� �̷��� �����Ѵ�. ���� �� �����ǿ����� ������ �ް� �ʹٸ�
	//�� �ּҸ� inet_addr�Լ��� �̿��� ������ �ȴ�.
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//������ ������ ���� �ּ� ������ cIOCompletionPort ������ �����Ѵ�.
	int nRet = bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		printf("[����] bind()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	//���� ��û�� �޾Ƶ��̱� ���� cIOCompletionPort������ ����ϰ� 
	//���Ӵ��ť�� 5���� ���� �Ѵ�.
	nRet = listen(mListenSocket, 5);
	if (0 != nRet)
	{
		printf("[����] listen()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	printf("���� ��� ����..\n");
	return true;
}


bool NetService::StartServer(const unsigned int maxClientCount)
{
	CreateClient(maxClientCount);

	//CompletionPort��ü ���� ��û�� �Ѵ�.
	auto ret = mIocp.CreateNewIocp(MAX_WORKERTHREAD);
	if(!ret)
	{
		return false;
	}
	
	
	//mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKERTHREAD);
	//if (NULL == mIOCPHandle)
	//{
	//	printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
	//	return false;
	//}

	//���ӵ� Ŭ���̾�Ʈ �ּ� ������ ������ ����ü
	ret = CreateWokerThread();
	if (!ret) {
		return false;
	}

	ret = CreateAccepterThread();
	if (ret) {
		return false;
	}

	printf("���� ����\n");
	return true;
}

void NetService::DestroyThread()
{
	mIsWorkerRun = false;
	CloseHandle(mIOCPHandle);

	for (auto& th : mIOWorkerThreads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	//Accepter �����带 �����Ѵ�.
	mIsAccepterRun = false;
	closesocket(mListenSocket);

	if (mAccepterThread.joinable())
	{
		mAccepterThread.join();
	}
}


void NetService::CreateClient(const unsigned int maxClientCount)
{
	for (UINT32 i = 0; i < maxClientCount; ++i)
	{
		mClientInfos.emplace_back();
	}
}

bool NetService::CreateWokerThread()
{
	unsigned int uiThreadId = 0;
	//WaingThread Queue�� ��� ���·� ���� ������� ���� ����Ǵ� ���� : (cpu���� * 2) + 1 
	for (int i = 0; i < MAX_WORKERTHREAD; i++)
	{
		mIOWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	printf("WokerThread ����..\n");
	return true;
}

bool NetService::CreateAccepterThread()
{
	mAccepterThread = std::thread([this]() { AccepterThread(); });

	printf("AccepterThread ����..\n");
	return true;
}


RemoteClient* NetService::GetEmptyClientInfo()
{
	for (auto& client : mClientInfos)
	{
		if (INVALID_SOCKET == client.stRemoteSock)
		{
			return &client;
		}
	}

	return nullptr;
}


bool NetService::BindIOCompletionPort(RemoteClient* pClientInfo)
{
	//socket�� pClientInfo�� CompletionPort��ü�� �����Ų��.

	auto ret = mIocp.AddDeviceRemoteSocket(pClientInfo);
	if (!ret)
	{
		return false;
	}

	//auto hIOCP = CreateIoCompletionPort((HANDLE)pClientInfo->stRemoteSock
	//	, mIOCPHandle
	//	, (ULONG_PTR)(pClientInfo), 0);

	//if (NULL == hIOCP || mIOCPHandle != hIOCP)
	//{
	//	printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
	//	return false;
	//}

	return true;
}


bool NetService::BindRecv(RemoteClient* pClientInfo)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	pClientInfo->stRecvOver.m_wsaBuf.len = MAX_SOCKBUF;
	pClientInfo->stRecvOver.m_wsaBuf.buf = pClientInfo->stRecvOver.m_RecvBuf;
	pClientInfo->stRecvOver.m_eOperation = IOOperation::RECV;

	int nRet = WSARecv(pClientInfo->stRemoteSock,
		&(pClientInfo->stRecvOver.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (pClientInfo->stRecvOver),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}


bool NetService::SendMsg(RemoteClient* pClientInfo, char* pMsg, int nLen)
{
	DWORD dwRecvNumBytes = 0;

	//���۵� �޼����� ����
	CopyMemory(pClientInfo->stSendOver.m_SendBuf, pMsg, nLen);


	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	pClientInfo->stSendOver.m_wsaBuf.len = nLen;
	pClientInfo->stSendOver.m_wsaBuf.buf = pClientInfo->stSendOver.m_SendBuf;
	pClientInfo->stSendOver.m_eOperation = IOOperation::SEND;

	pClientInfo->stSendOver.m_SendBuf[nLen] = NULL;

	int nRet = WSASend(pClientInfo->stRemoteSock,
		&(pClientInfo->stSendOver.m_wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED) & (pClientInfo->stSendOver),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSASend()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}


	return true;
}


void NetService::WokerThread()
{
	//CompletionKey�� ���� ������ ����
	RemoteClient* pClientInfo = NULL;
	//�Լ� ȣ�� ���� ����
	bool bSuccess = TRUE;
	//Overlapped I/O�۾����� ���۵� ������ ũ��
	unsigned long dwIoSize = 0;
	//I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
	LPOVERLAPPED lpOverlapped = NULL;

	while (mIsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(mIocp.m_workIocp,
			&dwIoSize,					// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pClientInfo,		// CompletionKey
			&lpOverlapped,				// Overlapped IO ��ü
			INFINITE);					// ����� �ð�

		//����� ������ ���� �޼��� ó��..
		if (bSuccess == false && 0 == dwIoSize && NULL == lpOverlapped)
		{
			mIsWorkerRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		//client�� ������ ��������..			
		if (FALSE == bSuccess || (0 == dwIoSize && TRUE == bSuccess))
		{
			printf("socket(%d) ���� ����\n", (int)pClientInfo->stRemoteSock);
			CloseSocket(pClientInfo);
			continue;
		}


		CustomOverEx* pOverlappedEx = (CustomOverEx*)lpOverlapped;

		//Overlapped I/O Recv�۾� ��� �� ó��
		if (IOOperation::RECV == pOverlappedEx->m_eOperation)
		{
			pOverlappedEx->m_RecvBuf[dwIoSize] = NULL;
			printf("[����] bytes : %d , msg : %s\n", dwIoSize, pOverlappedEx->m_RecvBuf);
			//Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
			SendMsg(pClientInfo, pOverlappedEx->m_RecvBuf, dwIoSize);
			BindRecv(pClientInfo);
		}
		//Overlapped I/O Send�۾� ��� �� ó��
		else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
		{
			printf("[�۽�] bytes : %d , msg : %s\n", dwIoSize, pOverlappedEx->m_SendBuf);

		}
		//���� ��Ȳ
		else
		{
			printf("socket(%d)���� ���ܻ�Ȳ\n", (int)pClientInfo->stRemoteSock);
		}
	}
}


void NetService::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);

	while (mIsAccepterRun)
	{
		//������ ���� ����ü�� �ε����� ���´�.
		RemoteClient* pClientInfo = GetEmptyClientInfo();
		if (NULL == pClientInfo)
		{
			printf("[����] Client Full\n");
			return;
		}

		//Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���.
		pClientInfo->stRemoteSock = accept(mListenSocket, (SOCKADDR*)&stClientAddr, &nAddrLen);
		if (INVALID_SOCKET == pClientInfo->stRemoteSock)
		{
			continue;
		}

		//I/O Completion Port��ü�� ������ �����Ų��.
		bool bRet = BindIOCompletionPort(pClientInfo);
		if (false == bRet)
		{
			return;
		}

		//Recv Overlapped I/O�۾��� ��û�� ���´�.
		bRet = BindRecv(pClientInfo);
		if (false == bRet)
		{
			return;
		}

		char clientIP[32] = { 0, };
		inet_ntop(AF_INET, &(stClientAddr.sin_addr), clientIP, 32 - 1);
		printf("Ŭ���̾�Ʈ ���� : IP(%s) SOCKET(%d)\n", clientIP, (int)pClientInfo->stRemoteSock);

		//Ŭ���̾�Ʈ ���� ����
		++mClientCnt;
	}
}


void NetService::CloseSocket(RemoteClient* pClientInfo, bool bIsForce)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ� ���� ���� ��Ų��. ���� : ������ �ս��� ������ ���� 
	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
	shutdown(pClientInfo->stRemoteSock, SD_BOTH);

	//���� �ɼ��� �����Ѵ�.
	setsockopt(pClientInfo->stRemoteSock, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//���� ������ ���� ��Ų��. 
	closesocket(pClientInfo->stRemoteSock);

	pClientInfo->stRemoteSock = INVALID_SOCKET;
}