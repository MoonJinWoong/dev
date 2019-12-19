#include "NetService.h"


#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP


bool NetService::InitSocket()
{
	WSADATA wsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (0 != nRet)
	{
		LOG(ERROR) << "WSAStartUp";
		return false;
	}

	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == mListenSocket)
	{
		LOG(ERROR) << "WSASocket";
		return false;
	}

	LOG(INFO) << "[SUCCESS] InitSocket";
	return true;
}

bool NetService::BindandListen()
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	auto port = Config::LoadNetConfig(CATEGORY_NET, PORT);

	stServerAddr.sin_port = htons(port);
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int nRet = ::bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		LOG(ERROR) << "bind";
		return false;
	}

	auto backlog = Config::LoadNetConfig(CATEGORY_NET, BACKLOG);
	nRet = ::listen(mListenSocket, backlog);
	if (0 != nRet)
	{
		LOG(ERROR) << "listen";
		return false;
	}

	LOG(INFO) << "[SUCCESS] BindandListen";
	return true;
}

bool NetService::StartNetService()
{
	auto mMaxWorkThreadCnt = Config::LoadNetConfig(CATEGORY_NET, MAX_WORK_THEAD_CNT);
	if (!mIocpService.CreateNewIocp(mMaxWorkThreadCnt))
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
	if (!CreateSessionPool())
	{
		return false;
	}

	LOG(INFO) << "[SUCCESS] StartNetService";
	return true;
}

void NetService::DestroyThread()
{
	mWorkerRun = false;
	CloseHandle(mIocpService.GetIocp());

	for (auto& th : mIOWorkerThreads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	mSendRun = false;
	if (mSendThread.joinable())
	{
		mSendThread.join();
	}
}

bool NetService::CreateSessionPool()
{
	mMaxSessionCnt = Config::LoadNetConfig(CATEGORY_NET, MAX_SESSION_COUNT);
	for (auto i = 0; i < mMaxSessionCnt; ++i)
	{
		auto pSession = new RemoteSession;
		pSession->SetUniqueId(i);
		if (!pSession->AcceptReady(mListenSocket))
		{
			LOG(ERROR) << "AcceptReady";
			return false;
		}
		mSessionPool.push_back(pSession);
	}
	return true;
}

bool NetService::CreateWokerThread()
{
	auto maxWorker = Config::LoadNetConfig(CATEGORY_NET, MAX_WORK_THEAD_CNT);
	mWorkerRun = true;
	for (auto i = 0; i < maxWorker; i++)
	{
		mIOWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	LOG(INFO) << "[SUCCESS] CreateWokerThread";
	return true;
}

bool NetService::CreateSendThread()
{
	mSendRun = true;
	mSendThread = std::thread([this]() { SendThread(); });
	LOG(INFO) << "[SUCCESS] CreateSendThread";
	return true;
}

RemoteSession* NetService::GetEmptySession()
{
	for (auto& client : mSessionPool)
	{
		if (INVALID_SOCKET == client->GetSock())
		{
			return client;
		}
	}
	return nullptr;
}

void NetService::DoAcceptFinish(unsigned int uid)
{
	auto pSession = GetSessionByIdx(uid);

	if (mIocpService.AddDeviceRemoteSocket(pSession))
	{
		pSession->AcceptFinish(mIocpService.GetIocp());
		ThrowLogicConnection(pSession->GetUniqueId(),PACKET_TYPE::CONNECTION);
	}
	else
	{
		KickSession(pSession,IO_TYPE::ACCEPT);
	}
}
void NetService::DoRecvFinish(CustomOverEx* pOver,unsigned long ioSize)
{
	auto session = GetSessionByIdx(pOver->mUid);	
	ThrowLogicRecv(pOver, ioSize);
}

void NetService::DoSend(RemoteSession* pSession,unsigned long size)
{
	pSession->SendFinish(size);
}

void NetService::WokerThread()
{
	RemoteSession* pSession = nullptr;
	bool bSuccess = true;
	unsigned long dwIoSize = 0;
	LPOVERLAPPED lpOverlapped = nullptr;

	while (mWorkerRun)
	{
		Iocp::IocpEvents events;
		mIocpService.GQCSEx(events, 100);

		for (int i = 0; i < events.m_eventCount; ++i)
		{
			auto Over = reinterpret_cast<CustomOverEx*>(events.m_IoArray[i].lpOverlapped);
			auto ioSize = events.m_IoArray[i].dwNumberOfBytesTransferred;
			pSession = reinterpret_cast<RemoteSession*>(events.m_IoArray[i].lpCompletionKey);

			if (0 >= ioSize && IO_TYPE::ACCEPT != Over->mIoType)
			{
				KickSession(pSession, Over->mIoType);
				continue;
			}

			switch (Over->mIoType)
			{
			case IO_TYPE::ACCEPT:
				DoAcceptFinish(Over->mUid);
				break;
			case IO_TYPE::RECV:
				DoRecvFinish(Over, ioSize);
				break;
			case IO_TYPE::SEND:
				DoSend(pSession, ioSize);
				break;
			default:
				LOG(ERROR) << "Bad Operation : " << (int)pSession->GetSock();
				break;
			}
		}
	}
}


//TODO 구조 바꿔보자.
void NetService::SendThread()
{
	while (mSendRun)
	{
		for (auto client : mSessionPool)
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

void NetService::KickSession(RemoteSession* pSession, IO_TYPE ioType)
{
	if (ioType == IO_TYPE::RECV)
		pSession->GetIoRef().DecRecvCount();
	else if(ioType == IO_TYPE::SEND)
		pSession->GetIoRef().DecSendCount();
	else
		pSession->GetIoRef().DecAcptCount();


	if (pSession->DisconnectFinish(mListenSocket))
	{
		std::lock_guard<std::mutex> guard(mLock);

		ThrowDisConnectProcess(pSession);
		pSession->Init();
		pSession->AcceptReady(mListenSocket);
	}

}

void NetService::ThrowDisConnectProcess(RemoteSession* pSession)
{
	pSession->CloseSocket();
	auto uniqueId = pSession->GetUniqueId();
	ThrowLogicConnection(uniqueId, PACKET_TYPE::DISCONNECTION);
}

bool NetService::SendMsg(unsigned int uniqueId, unsigned int size, char* pData)
{
	if (uniqueId < 0 || uniqueId >= mMaxSessionCnt)
	{
		return false;
	}


	auto session = GetSessionByIdx(uniqueId);
	session->SendReady(size, pData);
}