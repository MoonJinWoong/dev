
#include "NetService.h"

bool NetService::InitSocket()
{
	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		std::cout << "[err] WSAStartUP() fail..." << WSAGetLastError();
		return false;
	}

	//���������� TCP , Overlapped I/O ������ ����
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mListenSocket)
	{
		std::cout << "[err] WSASocket() fail..." << WSAGetLastError();
		return false;
	}

	std::cout << "[Success] InitSocket() "<<std::endl;
	return true;
}

bool NetService::BindandListen(u_Int nBindPort)
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(nBindPort); //���� ��Ʈ�� �����Ѵ�.		
	//� �ּҿ��� ������ �����̶� �޾Ƶ��̰ڴ�.
	//���� ������� �̷��� �����Ѵ�. ���� �� �����ǿ����� ������ �ް� �ʹٸ�
	//�� �ּҸ� inet_addr�Լ��� �̿��� ������ �ȴ�.
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//������ ������ ���� �ּ� ������ cIOCompletionPort ������ �����Ѵ�.
	int nRet = ::bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		std::cout << "[err] bind() fail..." << WSAGetLastError();
		return false;
	}

	//���� ��û�� �޾Ƶ��̱� ���� cIOCompletionPort������ ����ϰ� 
	//���Ӵ��ť�� 5���� ���� �Ѵ�.
	nRet = ::listen(mListenSocket, 5);
	if (0 != nRet)
	{
		std::cout << "[err] listen() fail..." << WSAGetLastError();
		return false;
	}

	std::cout << "[Success] BindandListen() " << std::endl;
	return true;
}

bool NetService::StartNetService(u_Int maxClientCount)
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

	if (!CreateSendThread())
	{
		return false;
	}

	std::cout << "[Success] StartNetService() " << std::endl;
	return true;
}

void NetService::DestroyThread()
{
	mIsWorkerRun = false;
	CloseHandle(mIocp.m_workIocp);

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

	// send thread ���� 
	mIsSendThreadRun = false;

	if (mSendThread.joinable())
	{
		mSendThread.join();
	}
}

void NetService::CreateClient(u_Int maxClientCount)
{
	for (auto i = 0; i < maxClientCount; ++i)
	{
		auto pSession = new RemoteSession;
		pSession->SetUniqueId(i);
		mVecSessions.emplace_back(pSession);
	}
}

bool NetService::CreateWokerThread()
{
	unsigned int uiThreadId = 0;
	//WaingThread Queue�� ��� ���·� ���� ������� ���� ����Ǵ� ���� : (cpu���� * 2) + 1 
	for (auto i = 0; i < MAX_WORKERTHREAD; i++)
	{
		mIOWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	std::cout << "[Success] WokerThread() " << std::endl;
	return true;
}

bool NetService::CreateAccepterThread()
{
	mAccepterThread = std::thread([this]() { AccepterThread(); });

	std::cout << "[Success] AccepterThread() " << std::endl;
	return true;
}

bool NetService::CreateSendThread()
{
	mSendThread = std::thread([this]() { SendThread(); });
	std::cout << "[Success] SendThread() " << std::endl;
	return true;
}

RemoteSession* NetService::GetEmptyClientInfo()
{
	for (auto& client : mVecSessions)
	{
		if (INVALID_SOCKET == client->GetSock())
		{
			return client;
		}
	}
	return nullptr;
}


bool NetService::DoRecv(RemoteSession* pSession)
{
	return pSession->RecvMsg();
}

void NetService::DoSend(RemoteSession* pSessoin)
{
	//pSessoin->SendPop();
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
			//DoSend(pSession, dwIoSize);
			
			//recv
			DoRecv(pSession);
		}
		//Overlapped I/O Send�۾� ��� �� ó��
		else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
		{
			int a = 0;
			//DoSend(pSession);
		}
		//���� ��Ȳ
		else
		{
			std::cout << "[Err] SocketFd : " << (int)pSession->GetSock() << std::endl;
		}
	}
}

void NetService::SendThread()
{
	while (mIsSendThreadRun)
	{
		if (!mSendQ.empty())
		{
			auto_lock guard(mSendLock);

			auto uniqueId = mSendQ.front();
			mSendQ.pop();

			auto session = GetSessionByIdx(uniqueId);
			unsigned long byte = 0;


			auto sendOver = new CustomOverEx;
			ZeroMemory(sendOver, sizeof(CustomOverEx));
			sendOver->m_wsaBuf.len = sizeof(session->mOverEx.m_SendBuf);
			sendOver->m_wsaBuf.buf = new char[sizeof(session->mOverEx.m_SendBuf)];
			CopyMemory(sendOver->m_wsaBuf.buf, session->mOverEx.m_SendBuf, sizeof(session->mOverEx.m_SendBuf));
			sendOver->m_eOperation = IOOperation::SEND;
			memset(session->mOverEx.m_SendBuf, 0, MAX_SOCKBUF);


			//TODO �̰͵� ©���°� ó�����־�� �ϴ°� �ƴұ�? ��� ������ ���.
			int nRet = WSASend(session->GetSock(),
				&(sendOver->m_wsaBuf),
				1,
				&byte,
				0,
				(LPWSAOVERLAPPED)sendOver,
				NULL);

			printf("[Send] Byte: %d \n", byte);

			if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
			{
				std::cout << " Err WSASend() code: " << WSAGetLastError() << std::endl;
			}
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
			std::cout << "[Err] Client Pull... UU " << std::endl;
			return;
		}

		//Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���.
		pClientInfo->mRemoteSock = accept(mListenSocket, (SOCKADDR*)&stClientAddr, &nAddrLen);
		
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


		auto id = pClientInfo->GetUniqueId();
		OnAccept(id);

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


bool NetService::SendMsg(u_Int uniqueId, u_Int size, char* pData)
{
	auto pSession = GetSessionByIdx(uniqueId);
	CopyMemory(pSession->mOverEx.m_SendBuf, pData, size);

	auto_lock guard(mSendLock);
	mSendQ.push(uniqueId);

	return true;
}