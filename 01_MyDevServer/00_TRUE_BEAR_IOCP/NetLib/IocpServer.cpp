
#include "IocpServer.h"
#include "Session.h"
#include <process.h>
#include <iostream>

using namespace std;

//unsigned int WINAPI CallWorkerThread(LPVOID p)
//{
//	IocpServer* pOverlappedEvent = (IocpServer*)p;
//	pOverlappedEvent->WorkerThread();
//	return 0;
//}

IocpServer::IocpServer()
{
	this->InitBase();
	m_bWorkerThread = true;
	m_bAccept = true;

	mSessionMgr = make_unique<SessionMgr>();

}


IocpServer::~IocpServer()
{
	::closesocket(m_listenSocket);
}

bool IocpServer::InitAndCreateSocket()
{
	WSADATA wsaData;
	int nResult;
	// winsock 2.2 버전으로 초기화
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (nResult != 0)
	{
		printf_s("[ERROR] winsock 초기화 실패\n");
		return false;
	}

	// 소켓 생성
	m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		printf_s("[ERROR] 소켓 생성 실패\n");
		return false;
	}

	// 서버 정보 설정
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 소켓 설정
	nResult = ::bind(m_listenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN));
	if (nResult == SOCKET_ERROR)
	{
		printf_s("[ERROR] bind 실패\n");
		closesocket(m_listenSocket);
		WSACleanup();
		return false;
	}

	// 수신 대기열 생성
	nResult = listen(m_listenSocket, 5);
	if (nResult == SOCKET_ERROR)
	{
		printf_s("[ERROR] listen 실패\n");
		closesocket(m_listenSocket);
		WSACleanup();
		return false;
	}

	return true;
}

bool IocpServer::Run()
{
	// listen_socket 생성 , bind and listen
	this->InitAndCreateSocket();

	// Completion Port 객체 생성
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
	if (m_hIOCP == nullptr)
	{
		printf_s("[INFO] CreateIoCompletionPort fail ! \n");
	}


	// worker thread 생성
	vector<thread*> worker;
	//worker.reserve(8);
	for (int i = 0; i < 8; ++i)
	{
		worker.push_back(new thread{ &IocpServer::WorkerThread,this });
	}

	// accept thread 생성
	thread AcceptThread{ &IocpServer::AcceptThread,this };

	//  join 
	for (auto wor : worker)
	{ 
		wor->join();  
		delete wor; 
	}
	worker.clear();
	AcceptThread.join();


	CloseServer();


	return true;
}

// 구현중인거 
void IocpServer::WorkerThread()
{


	while (m_bWorkerThread)
	{
		IoData* ioData = nullptr;
		Session* session = nullptr;
		DWORD    size = 0;

		auto ret = GetQueuedCompletionStatus(m_hIOCP,
			&size,							// 실제로 전송된 바이트
			(PULONG_PTR)& session,			// completion key
			(LPOVERLAPPED*)& ioData,		// overlapped I/O 객체
			INFINITE						// 대기할 시간
		);


		if (!ret)
		{
			continue;
		}

		if (size == 0)
		{
			cout << "[INFO]  접속 끊김 session ID : " << session->m_ID << endl;
			mSessionMgr->closeSession(session);
			continue;
		}

		switch(ioData->ioType)
		{

			case IO_WRITE:
			{
				session->checkSend(size);
				continue;
			}
	
			case IO_READ:
			{
				cout<<ioData->msgBuffer[0] << endl;
				cout << ioData->msgBuffer[1] << endl;
				cout << ioData->msgBuffer[2] << endl;
				cout << ioData->msgBuffer[3] << endl;

				// 여기서부터 다시 
			
				continue;
			}
			case IO_ERROR:
			{
				mSessionMgr->closeSession(session);
				continue;
			}

		}
	}
}
//void IocpServer::WorkerThread()
//{
//	// 함수 호출 성공 여부
//	BOOL	bResult;
//	int		nResult;
//	// Overlapped I/O 작업에서 전송된 데이터 크기
//	DWORD	recvBytes;
//	DWORD	sendBytes;
//	// Completion Key를 받을 포인터 변수
//	stSOCKETINFO* pCompletionKey;
//	// I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
//	stSOCKETINFO* pSocketInfo;
//	// 
//	DWORD	dwFlags = 0;
//
//	while (m_bWorkerThread)
//	{
//		
//		/**
//		 * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
//		 * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와
//		 * 뒷처리를 함
//		 */
//		bResult = GetQueuedCompletionStatus(m_hIOCP,
//			&recvBytes,				// 실제로 전송된 바이트
//			(PULONG_PTR)& pCompletionKey,	// completion key
//			(LPOVERLAPPED*)& pSocketInfo,			// overlapped I/O 객체
//			INFINITE				// 대기할 시간
//		);
//
//		if (!bResult && recvBytes == 0)
//		{
//			printf_s("[INFO] socket(%d) 접속 끊김\n", pSocketInfo->socket);
//			closesocket(pSocketInfo->socket);
//			free(pSocketInfo);
//			continue;
//		}
//
//		pSocketInfo->dataBuf.len = recvBytes;
//
//		if (recvBytes == 0)
//		{
//			closesocket(pSocketInfo->socket);
//			free(pSocketInfo);
//			continue;
//		}
//		else
//		{
//			printf_s("[INFO] 메시지 수신- Bytes : [%d], Msg : [%s]\n",
//				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);
//
//			// 클라이언트의 응답을 그대로 송신			
//			nResult = WSASend(
//				pSocketInfo->socket,
//				&(pSocketInfo->dataBuf),
//				1,
//				&sendBytes,
//				dwFlags,
//				NULL,
//				NULL
//			);
//
//			if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
//			{
//				printf_s("[ERROR] WSASend 실패 : ", WSAGetLastError());
//			}
//
//			printf_s("[INFO] 메시지 송신 - Bytes : [%d], Msg : [%s]\n",
//				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);
//
//			// stSOCKETINFO 데이터 초기화
//			ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
//			pSocketInfo->dataBuf.len = MAX_BUFFER;
//			pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
//			ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
//			pSocketInfo->recvBytes = 0;
//			pSocketInfo->sendBytes = 0;
//
//			dwFlags = 0;
//
//			// 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
//			nResult = WSARecv(
//				pSocketInfo->socket,
//				&(pSocketInfo->dataBuf),
//				1,
//				&recvBytes,
//				&dwFlags,
//				(LPWSAOVERLAPPED) & (pSocketInfo->overlapped),
//				NULL
//			);
//
//			if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
//			{
//				printf_s("[ERROR] WSARecv 실패 : ", WSAGetLastError());
//			}
//		}
//	}
//}

void IocpServer::ResiterSession(SOCKET acceptedSocket, SOCKADDR_IN addrInfo)
{
	Session* session = new Session();
	if (session == nullptr)
	{
		cout << "session alloc fail" << endl;
		return;
	}

	if (!session->resisterSession(acceptedSocket, addrInfo))
	{
		delete(session);
		cout << "session resister fail" << endl;
		return;
	}

	if (!mSessionMgr->addSession(session))
	{
		delete(session);
		cout << "session manager add fail" << endl;
		return;
	}

	session->mIoData[IO_READ].Clear();

	auto ret  = CreateIoCompletionPort(
		reinterpret_cast<HANDLE>(acceptedSocket), m_hIOCP, (ULONG_PTR)&(*session), NULL);

	if (!ret)
	{
		delete(session);
		return;
	}

	session->recvReady();
}


void IocpServer::AcceptThread()
{
	int nResult;
	// 클라이언트 정보
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET acceptedSocket;



	printf_s("[INFO] Accepting...\n");

	// 클라이언트 접속을 받음
	while (m_bAccept)
	{
		acceptedSocket = WSAAccept(
			m_listenSocket, (struct sockaddr*) & clientAddr, &addrLen, NULL, NULL
		);

		if (acceptedSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] Accept 실패\n");
			return;
		}

		this->ResiterSession(acceptedSocket, clientAddr);
	}
}

void IocpServer::CloseServer()
{
	closesocket(m_listenSocket);
	WSACleanup();
}

