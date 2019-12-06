
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

	//CompletionPort ��ü ���� ��û�� �Ѵ�.
	if (!mIocpService.CreateNewIocp(MAX_WORKERTHREAD))
	{
		return false;
	}
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
	mWorkerRun = false;
	CloseHandle(mIocpService.mIocp);

	for (auto& th : mIOWorkerThreads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	// send thread ���� 
	mSendRun = false;

	if (mSendThread.joinable())
	{
		mSendThread.join();
	}
}

bool NetService::CreateSessionPool(unsigned int maxClientCount)
{
	for (auto i = 0; i < maxClientCount; ++i)
	{
		auto pSession = new RemoteSession;
		pSession->SetUniqueId(i);
		if (!pSession->AcceptReady(mListenSocket))
		{
			std::cout << "AcceptReady fail" << std::endl;
			return false;
		}
		mVecSessions.emplace_back(pSession);
	}
	return true;
}

bool NetService::CreateWokerThread()
{
	mWorkerRun = true;
	for (auto i = 0; i < MAX_WORKERTHREAD; i++)
	{
		mIOWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	std::cout << "[Success] WokerThread() " << std::endl;
	return true;
}

bool NetService::CreateSendThread()
{
	mSendRun = true;
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

void NetService::DoAcceptFinish(RemoteSession* pSession,unsigned int uid)
{
	pSession = GetSessionByIdx(uid);

	if (mIocpService.AddDeviceRemoteSocket(pSession))
	{
		pSession->AcceptFinish(mIocpService.mIocp);
		ThrowLogicConnection(pSession->GetUniqueId(),PACKET_TYPE::CONNECTION);
	}
	else
	{
		KickSession(pSession, true);
	}
}
void NetService::DoRecvFinish(CustomOverEx* pOver,unsigned long ioSize)
{
	auto session = GetSessionByIdx(pOver->mUid);	
	ThrowLogicRecv(pOver, ioSize);
	session->RecvMsg();
}

void NetService::DoSend(RemoteSession* pSessoin)
{
}

void NetService::WokerThread()
{
	RemoteSession* pSession = nullptr;
	bool bSuccess = true;
	unsigned long dwIoSize = 0;
	LPOVERLAPPED lpOverlapped = nullptr;

	while (mWorkerRun)
	{

		bSuccess = GetQueuedCompletionStatus(mIocpService.mIocp,
			&dwIoSize,					// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pSession,		// CompletionKey
			&lpOverlapped,				// Overlapped IO ��ü
			INFINITE);					// ����� �ð�


		if (bSuccess == true && 0 == dwIoSize && nullptr == lpOverlapped)
		{
			mWorkerRun = false;
			continue;
		}

		if (nullptr == lpOverlapped)
		{
			continue;
		}

		CustomOverEx* pOverlappedEx = (CustomOverEx*)lpOverlapped;

		if (bSuccess == false || (0 == dwIoSize && IOOperation::ACCEPT != pOverlappedEx->mIoType))
		{
			KickSession(pSession);
			continue;
		}


		if (IOOperation::ACCEPT == pOverlappedEx->mIoType)
		{
			DoAcceptFinish(pSession,pOverlappedEx->mUid);
		}
		else if (IOOperation::RECV == pOverlappedEx->mIoType)
		{
			pOverlappedEx->mUid = pSession->GetUniqueId();
			DoRecvFinish(pOverlappedEx, dwIoSize);
		}
		else if (IOOperation::SEND == pOverlappedEx->mIoType)
		{
			pSession->SendFinish(dwIoSize);
		}
		else
		{
			std::cout << "[Err] Operation : " << (int)pSession->GetSock() << std::endl;
		}

	}
}



void NetService::SendThread()
{
	while (mSendRun)
	{
		for (auto client : mVecSessions)
		{
			if (!client->IsLive())
			{
				continue;
			}

			if (!client->SendPacket())
			{
				continue;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}
}



void NetService::KickSession(RemoteSession* pSession, bool isForce)
{
	if (!pSession->IsLive())
	{
		return;
	}
	
	//std::lock_guard<std::mutex> guard(mLock);
	pSession->UnInit(isForce, mListenSocket);
	--mSessionCnt;
	auto uniqueId = pSession->GetUniqueId();
	ThrowLogicConnection(uniqueId,PACKET_TYPE::DISCONNECTION);
}


bool NetService::SendMsg(unsigned int uniqueId, unsigned int size, char* pData)
{
	// �� �����Ǽ� �´�.

	if (uniqueId < 0 || uniqueId >= mMaxSessionCnt)
	{
		return false;
	}
	auto pClient = GetSessionByIdx(uniqueId);
	return pClient->SendReady(size, pData);
}