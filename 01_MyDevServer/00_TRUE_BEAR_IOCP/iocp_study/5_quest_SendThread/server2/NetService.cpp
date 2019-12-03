
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

	//연결지향형 TCP , Overlapped I/O 소켓을 생성
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
	stServerAddr.sin_port = htons(nBindPort); //서버 포트를 설정한다.		
	//어떤 주소에서 들어오는 접속이라도 받아들이겠다.
	//보통 서버라면 이렇게 설정한다. 만약 한 아이피에서만 접속을 받고 싶다면
	//그 주소를 inet_addr함수를 이용해 넣으면 된다.
	stServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//위에서 지정한 서버 주소 정보와 cIOCompletionPort 소켓을 연결한다.
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

	//CompletionPort객체 생성 요청을 한다.
	if (!mIocpService.CreateNewIocp(MAX_WORKERTHREAD))
	{
		return false;
	}

	// 리슨 소켓을 iocp에 등록
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

	// send thread 종료 
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
	//WaingThread Queue에 대기 상태로 넣을 쓰레드들 생성 권장되는 개수 : (cpu개수 * 2) + 1 
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
	//CompletionKey를 받을 포인터 변수
	RemoteSession* pSession = nullptr;
	//함수 호출 성공 여부
	bool bSuccess = true;
	//Overlapped I/O작업에서 전송된 데이터 크기
	unsigned long dwIoSize = 0;
	//I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터
	LPOVERLAPPED lpOverlapped = nullptr;

	while (mIsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(mIocpService.mIocp,
			&dwIoSize,					// 실제로 전송된 바이트
			(PULONG_PTR)&pSession,		// CompletionKey
			&lpOverlapped,				// Overlapped IO 객체
			INFINITE);					// 대기할 시간

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

		//client가 접속을 끊음		
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
				//TODO 정확하게 증가 하려면 락을 걸어야 할거 같다.
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
			// OS가 받은 recv 뒤처리(패킷 분해해조립하고 큐에 담기)
			pOverlappedEx->mUid = pSession->GetUniqueId();
			OnRecv(pOverlappedEx, dwIoSize);
			
			// 처리 하고 다시 RECVIO 걸어야함
			pSession->RecvMsg();
		}
		else if (IOOperation::SEND == pOverlappedEx->mIoType)
		{
			if (pOverlappedEx->mWSABuf.len != dwIoSize)
			{
				//TODO 모든 메세지를 전송하지 못한 상황. 재현이 어렵다..
			}
			else
			{
				// send 뒤처리
				pSession->SendFinish(dwIoSize);
			}
		}
		//예외
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
				// 접속 끊어준다
			}

			// 무조건 꺼내야 한다. 안그럼 다른 세션들도 막힘
			mSendQ.pop();
		}
	}
}

void NetService::CloseSocket(RemoteSession* pSession, bool bIsForce)
{
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown(pSession->GetSock(), SD_BOTH);

	//소켓 옵션을 설정한다.
	setsockopt(pSession->GetSock(), SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//소켓 연결을 종료 시킨다. 
	closesocket(pSession->GetSock());

	pSession->GetSock() = INVALID_SOCKET;
}


bool NetService::SendMsg(unsigned int uniqueId, unsigned int size, char* pData)
{

	// 다 조립되서 온다.
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

	//TODO 락을 걸어야 할거 같다.
	--mClientCnt;

	auto uniqueId = pSession->GetUniqueId();
	OnClose(uniqueId);
}
