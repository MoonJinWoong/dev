
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

	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mListenSocket)
	{
		std::cout << "[err] WSASocket() fail..." << WSAGetLastError();
		return false;
	}

	std::cout << "[Success] InitSocket() "<<std::endl;
	return true;
}

bool NetService::BindandListen(c_u_Int nBindPort)
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(nBindPort); //���� ��Ʈ�� �����Ѵ�.		
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);


	int nRet = ::bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		std::cout << "[err] bind() fail..." << WSAGetLastError();
		return false;
	}

	//TODO ���� �׽�Ʈ �Ҷ� ���Ӵ��ť �����غ� ��
	nRet = ::listen(mListenSocket, SOMAXCONN);
	if (0 != nRet)
	{
		std::cout << "[err] listen() fail..." << WSAGetLastError();
		return false;
	}

	std::cout << "[Success] BindandListen() " << std::endl;
	return true;
}

bool NetService::StartNetService(c_u_Int maxClientCount)
{

	// iocp ����
	if (!mIocp.CreateNewIocp(MAX_WORKERTHREAD))
	{
		return false;
	}

	// ���� ������ iocp�� ���
	if (!mIocp.AddDeviceListenSocket(mListenSocket))
	{
		return false;

	}

	// ��Ŀ ������ 
	if(!CreateWokerThread())
	{
		return false;
	}

	// ����Ǯ ���� , acceptEX �ɾ����
	if (!CreateSessions(maxClientCount))
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

	closesocket(mListenSocket);
}

bool NetService::CreateSessions(c_u_Int maxClientCount)
{
	for (auto i = 0; i < maxClientCount; ++i)
	{
		auto pSession = new RemoteSession;
		pSession->SetUniqueId(i);
		if (!pSession->AccpetAsync(mListenSocket))
			return false;
		mVecSessions.emplace_back(pSession);
	}
	return true;
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

RemoteSession* NetService::GetEmptySession()
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


void NetService::DoSend(RemoteSession* pSessoin)
{
	pSessoin->SendPopInWorker();
}

void NetService::WokerThread()
{
	RemoteSession* pSession = NULL;  // completeion key
	bool bSuccess = true;
	unsigned long dwIoSize = 0;
	LPOVERLAPPED lpOverlapped = NULL;

	while (mIsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(mIocp.m_workIocp,
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

		if (IOOperation::ACCEPT == pOverlappedEx->m_eOperation)
		{
			pSession = GetSessionByIdx(pOverlappedEx->mUid);
			if (mIocp.AddDeviceRemoteSocket(pSession))
			{
				//TODO ��Ȯ�ϰ� ���� �Ϸ��� ���� �ɾ�� �Ұ� ����.
				++mClientCnt;
				pSession->AcceptFinish(mIocp.m_workIocp);

				OnAccept(pOverlappedEx->mUid);
			}
			else
			{
				KickOutSession(pSession, true);
			}

		}
		else if (IOOperation::RECV == pOverlappedEx->m_eOperation)
		{
			// logic
			pOverlappedEx->m_RecvBuf[dwIoSize] = NULL;			
			OnRecv(pSession->GetUniqueId(), dwIoSize, pOverlappedEx->m_RecvBuf);
			
			// ����
			pSession->RecvStart();
		}
		else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
		{
			DoSend(pSession);
		}
		else
		{
			std::cout << "[Err] SocketFd : " << (int)pSession->GetSock() << std::endl;
		}
	}
}


void NetService::KickOutSession(RemoteSession* pSession, bool isForce)
{

	if (pSession->IsConnected() == false)
	{
		return;
	}
	pSession->Release(isForce, mListenSocket);
	
	//TODO ���� �ɾ�� �Ұ� ����.
	--mClientCnt;
	
	auto uniqueId = pSession->GetUniqueId();
	OnClose(uniqueId);
}


bool NetService::SendMsg(c_u_Int uniqueId, c_u_Int size, char* pData)
{
	std::cout << "session : " << uniqueId << "  size :" << size << std::endl;
	auto pSession = GetSessionByIdx(uniqueId);
	return pSession->SendPushInLogic(size, pData);
}

