
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

bool NetService::BindandListen(unsigned int nBindPort)
{
	SOCKADDR_IN		stServerAddr;
	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(nBindPort); 
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int nRet = ::bind(mListenSocket, (SOCKADDR*)&stServerAddr, sizeof(SOCKADDR_IN));
	if (0 != nRet)
	{
		std::cout << "[err] bind() fail..." << WSAGetLastError();
		return false;
	}

	//접속 요청을 받아들이기 위해 cIOCompletionPort소켓을 등록하고 
	//접속대기큐를 5개로 설정 한다.
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
		mSessionPool.emplace_back(pSession);
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
		IocpEvents events;
		mIocpService.GQCSEx(events, 100);

		for (int i = 0; i < events.m_eventCount; ++i)
		{
			auto Over = reinterpret_cast<CustomOverEx*>(events.m_IoArray[i].lpOverlapped);
			auto ioSize = events.m_IoArray[i].dwNumberOfBytesTransferred;
			pSession = (RemoteSession*)events.m_IoArray[i].lpCompletionKey;

			if (0 >= ioSize && IO_TYPE::ACCEPT != Over->mIoType)
			{
				KickSession(pSession);
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
				pSession->SendFinish(ioSize);
				break;
			default:
				std::cout << "[Err] Operation : " << (int)pSession->GetSock() << std::endl;
				break;
			}
		}
	}
}



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



void NetService::KickSession(RemoteSession* pSession, bool isForce)
{
	if (!pSession->IsLive())
	{
		return;
	}
	
	std::lock_guard<std::mutex> guard(mLock);
	pSession->UnInit(isForce, mListenSocket);
	--mSessionCnt;
	auto uniqueId = pSession->GetUniqueId();
	ThrowLogicConnection(uniqueId,PACKET_TYPE::DISCONNECTION);
}


bool NetService::SendMsg(unsigned int uniqueId, unsigned int size, char* pData)
{
	// 다 조립되서 온다.
	if (uniqueId < 0 || uniqueId >= mMaxSessionCnt)
	{
		return false;
	}
	auto pClient = GetSessionByIdx(uniqueId);
	return pClient->SendReady(size, pData);
}