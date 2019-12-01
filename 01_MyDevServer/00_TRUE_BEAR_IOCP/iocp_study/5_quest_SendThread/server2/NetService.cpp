
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

bool NetService::BindandListen(unsigned int nBindPort)
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

bool NetService::StartNetService(unsigned int maxClientCount)
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

void NetService::CreateClient(unsigned int maxClientCount)
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


void NetService::DoRecv(RemoteSession* pSession)
{

}

void NetService::DoSend(RemoteSession* pSessoin)
{
	//pSessoin->SendPop();
}

void NetService::WokerThread()
{
	//CompletionKey�� ���� ������ ����
	RemoteSession* pSession = nullptr;
	//�Լ� ȣ�� ���� ����
	bool bSuccess = true;
	//Overlapped I/O�۾����� ���۵� ������ ũ��
	unsigned long dwIoSize = 0;
	//I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������
	LPOVERLAPPED lpOverlapped = nullptr;

	while (mIsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(mIocp.m_workIocp,
			&dwIoSize,					// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pSession,		// CompletionKey
			&lpOverlapped,				// Overlapped IO ��ü
			INFINITE);					// ����� �ð�

		//����� ������ ���� �޼��� ó��..
		if (bSuccess == false && 0 == dwIoSize && nullptr == lpOverlapped)
		{
			mIsWorkerRun = false;
			continue;
		}

		if (nullptr == lpOverlapped)
		{
			continue;
		}

		//client�� ������ ��������..			
		if (false == bSuccess || (0 == dwIoSize && true == bSuccess))
		{
			OnClose(pSession->GetUniqueId());
			CloseSocket(pSession);
			continue;
		}


		CustomOverEx* pOverlappedEx = (CustomOverEx*)lpOverlapped;
		if (IOOperation::RECV == pOverlappedEx->m_eOperation)
		{
			// OS�� ���� recv ��ó��(��Ŷ �����������ϰ� ť�� ���)
			pOverlappedEx->mUid = pSession->GetUniqueId();
			OnRecv(pOverlappedEx, dwIoSize);
			
			// ó�� �ϰ� �ٽ� RECVIO �ɾ����
			pSession->RecvMsg();
		}
		else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
		{
			if (pOverlappedEx->m_wsaBuf.len != dwIoSize)
			{
				// ��� �޼��� �������� ���� ��Ȳ�� ����ؾ���
			}
			else
			{
				// send ��ó��
				pSession->SendFinish(dwIoSize);
			}
		}
		//����
		else
		{
			std::cout << "[Err] Operation : " << (int)pSession->GetSock() << std::endl;
		}

	}
}

void NetService::SendThread()
{
	while (true)
	{
		if (!mSendQ.empty())
		{
			auto& sendOver = mSendQ.front();
			auto session = GetSessionByIdx(sendOver->mUid);
			
			if (session->SendPacket(sendOver->mBuf.data(), sendOver->mSendPos))
			{
				// ���� �����ش�
			}

			// ������ ������ �Ѵ�. �ȱ׷� �ٸ� ���ǵ鵵 ����
			mSendQ.pop();
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
		if (!pClientInfo->RecvMsg())
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


bool NetService::SendMsg(unsigned int uniqueId, unsigned int size, char* pData)
{

	// �� �����Ǽ� �´�.
	std::lock_guard<std::mutex> guard(mSendLock);

	if (uniqueId < 0 || uniqueId >= mMaxSessionCnt)
	{
		return false;
	}

	CustomOverEx* SendOver = new CustomOverEx;

	SendOver->mUid = uniqueId;
	CopyMemory(SendOver->mBuf.data(), pData, size);
	SendOver->mSendPos += size;
	mSendQ.push(SendOver);
}