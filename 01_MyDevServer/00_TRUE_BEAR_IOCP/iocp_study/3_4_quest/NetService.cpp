
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

bool NetService::StartNetService(const unsigned int maxClientCount)
{
	CreateClient(maxClientCount);

	//CompletionPort��ü ���� ��û�� �Ѵ�.
	if (!mIocp.CreateNewIocp(MAX_WORKERTHREAD))
	{
		return false;
	}

	if(!CreateWokerThread())
	{
		return false;
	}

	if(!CreateAccepterThread())
	{
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
	for (int i = 0; i < maxClientCount; ++i)
	{
		RemoteSession pSession;
		pSession.SetUniqueId(i);
		mVecSessions.emplace_back(pSession);
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

RemoteSession* NetService::GetEmptyClientInfo()
{
	for (auto& client : mVecSessions)
	{
		if (INVALID_SOCKET == client.GetSock())
		{
			return &client;
		}
	}
	return nullptr;
}

bool NetService::BindIOCompletionPort(RemoteSession* pClientInfo)
{
	//socket�� pClientInfo�� CompletionPort��ü�� �����Ų��.

	auto ret = mIocp.AddDeviceRemoteSocket(pClientInfo);
	if (!ret)
	{
		return false;
	}
	return true;
}

bool NetService::DoRecv(RemoteSession* pClientInfo)
{
	return pClientInfo->RecvMsg();
}

bool NetService::DoSend(RemoteSession* pClientInfo, char* pMsg, int nLen)
{
	return pClientInfo->SendMsg(nLen, pMsg);
}

void NetService::WokerThread()
{
	//CompletionKey�� ���� ������ ����
	RemoteSession* pSession = NULL;
	//�Լ� ȣ�� ���� ����
	bool bSuccess = true;
	//Overlapped I/O�۾����� ���۵� ������ ũ��
	unsigned long dwIoSize = 0;
	//I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
	LPOVERLAPPED lpOverlapped = NULL;

	while (mIsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(mIocp.m_workIocp,
			&dwIoSize,					// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pSession,		// CompletionKey
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
		if (FALSE == bSuccess || (0 == dwIoSize && true == bSuccess))
		{
			OnClose(pSession->GetUniqueId());
			CloseSocket(pSession);
			continue;
		}


		CustomOverEx* pOverlappedEx = (CustomOverEx*)lpOverlapped;

		//Overlapped I/O Recv�۾� ��� �� ó��
		if (IOOperation::RECV == pOverlappedEx->m_eOperation)
		{
			pOverlappedEx->m_RecvBuf[dwIoSize] = NULL;			
			OnRecv(pSession->GetUniqueId(), dwIoSize, pOverlappedEx->m_RecvBuf);

			// echo
			DoSend(pSession, pOverlappedEx->m_RecvBuf, dwIoSize);
			
			//recv
			DoRecv(pSession);
		}
		//Overlapped I/O Send�۾� ��� �� ó��
		else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
		{
			std::cout << "[Send] Byte : " << dwIoSize << ", Msg :" << pOverlappedEx->m_SendBuf << std::endl;
		}
		//���� ��Ȳ
		else
		{
			std::cout << "[Err] SocketFd : " << (int)pSession->GetSock() << std::endl;
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
		RemoteSession* pClientInfo = GetEmptyClientInfo();
		if (NULL == pClientInfo)
		{
			printf("[����] Client Full\n");
			return;
		}

		//Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���.
		pClientInfo->GetSock() = accept(mListenSocket, (SOCKADDR*)&stClientAddr, &nAddrLen);
		if (INVALID_SOCKET == pClientInfo->GetSock())
		{
			continue;
		}

		//I/O Completion Port��ü�� ������ �����Ų��.
		if (!mIocp.AddDeviceRemoteSocket(pClientInfo))
		{
			return;
		}

		//Recv Overlapped I/O�۾��� ��û�� ���´�.
		if (!DoRecv(pClientInfo))
		{
			return;
		}



		OnAccept(pClientInfo->GetUniqueId());

		//Ŭ���̾�Ʈ ���� ����
		++mClientCnt;
	}
}

void NetService::CloseSocket(RemoteSession* pSession, bool bIsForce)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ� ���� ���� ��Ų��. ���� : ������ �ս��� ������ ���� 
	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
	shutdown(pSession->GetSock(), SD_BOTH);

	//���� �ɼ��� �����Ѵ�.
	setsockopt(pSession->GetSock(), SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//���� ������ ���� ��Ų��. 
	closesocket(pSession->GetSock());

	pSession->GetSock() = INVALID_SOCKET;
}