
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

	//CompletionPort��ü ���� ��û�� �Ѵ�.
	if (!mIocpService.CreateNewIocp(MAX_WORKERTHREAD))
	{
		return false;
	}

	// ���� ������ iocp�� ���
	if (!mIocpService.AddDeviceListenSocket(mListenSocket))
	{
		return false;

	}

	if(!CreateWokerThread())
	{
		return false;
	}

	if (!CreateSendThread())
	{
		return false;
	}

	if (!CreateSessionPool(maxClientCount))
	{
		return false;
	}


	std::cout << "[Success] StartNetService() " << std::endl;
	return true;
}

void NetService::DestroyThread()
{
	mIsWorkerRun = false;
	CloseHandle(mIocpService.mIocp);

	for (auto& th : mIOWorkerThreads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	// send thread ���� 
	mIsSendThreadRun = false;

	if (mSendThread.joinable())
	{
		mSendThread.join();
	}
}

bool NetService::CreateSessionPool(unsigned int maxClientCount)
{
	for (auto i = 0; i < maxClientCount; ++i)
	{
		std::cout << "asdfasdfasdfasdfsafd" << std::endl;

		auto pSession = new RemoteSession;
		pSession->SetUniqueId(i);
		if (!pSession->AcceptReady(mListenSocket))
			return false;
		mVecSessions.emplace_back(pSession);
	}
	true;
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
		bSuccess = GetQueuedCompletionStatus(mIocpService.mIocp,
			&dwIoSize,					// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pSession,		// CompletionKey
			&lpOverlapped,				// Overlapped IO ��ü
			INFINITE);					// ����� �ð�

		if (bSuccess == true && 0 == dwIoSize && NULL == lpOverlapped)
		{
			mIsWorkerRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		CustomOverEx* pOverlappedEx = (CustomOverEx*)lpOverlapped;

		//client�� ������ ����		
		if (bSuccess == false || (0 == dwIoSize && IOOperation::ACCEPT != pOverlappedEx->m_eOperation))
		{
			KickOutSession(pSession);
			continue;
		}


		if (IOOperation::ACCEPT == pOverlappedEx->mIoType)
		{
			pSession = GetSessionByIdx(pOverlappedEx->mUid);
			if (mIocpService.AddDeviceRemoteSocket(pSession))
			{
				//TODO ��Ȯ�ϰ� ���� �Ϸ��� ���� �ɾ�� �Ұ� ����.
				++mClientCnt;
				pSession->AcceptFinish(mIocpService.mIocp);

				OnAccept(pOverlappedEx->mUid);
			}
			else
			{
				KickOutSession(pSession, true);;
			}
			
		}
		else if (IOOperation::RECV == pOverlappedEx->mIoType)
		{
			// OS�� ���� recv ��ó��(��Ŷ �����������ϰ� ť�� ���)
			pOverlappedEx->mUid = pSession->GetUniqueId();
			OnRecv(pOverlappedEx, dwIoSize);
			
			// ó�� �ϰ� �ٽ� RECVIO �ɾ����
			pSession->RecvMsg();
		}
		else if (IOOperation::SEND == pOverlappedEx->mIoType)
		{
			if (pOverlappedEx->mWSABuf.len != dwIoSize)
			{
				//TODO ��� �޼����� �������� ���� ��Ȳ. ������ ��ƴ�..
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
			
			if (!session->SendPacket(sendOver->mBuf.data(), sendOver->mSendPos))
			{
				// ���� �����ش�
			}

			// ������ ������ �Ѵ�. �ȱ׷� �ٸ� ���ǵ鵵 ����
			mSendQ.pop();
		}
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

void NetService::KickOutSession(RemoteSession* pSession, bool isForce)
{
	if (pSession->IsLive() == false)
	{
		return;
	}
	pSession->Release(isForce, mListenSocket);

	//TODO ���� �ɾ�� �Ұ� ����.
	--mClientCnt;

	auto uniqueId = pSession->GetUniqueId();
	OnClose(uniqueId);
}
