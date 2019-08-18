
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
	// winsock 2.2 �������� �ʱ�ȭ
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (nResult != 0)
	{
		printf_s("[ERROR] winsock �ʱ�ȭ ����\n");
		return false;
	}

	// ���� ����
	m_listenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_listenSocket == INVALID_SOCKET)
	{
		printf_s("[ERROR] ���� ���� ����\n");
		return false;
	}

	// ���� ���� ����
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// ���� ����
	nResult = ::bind(m_listenSocket, (struct sockaddr*) & serverAddr, sizeof(SOCKADDR_IN));
	if (nResult == SOCKET_ERROR)
	{
		printf_s("[ERROR] bind ����\n");
		closesocket(m_listenSocket);
		WSACleanup();
		return false;
	}

	// ���� ��⿭ ����
	nResult = listen(m_listenSocket, 5);
	if (nResult == SOCKET_ERROR)
	{
		printf_s("[ERROR] listen ����\n");
		closesocket(m_listenSocket);
		WSACleanup();
		return false;
	}

	return true;
}

bool IocpServer::Run()
{
	// listen_socket ���� , bind and listen
	this->InitAndCreateSocket();

	// Completion Port ��ü ����
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);
	if (m_hIOCP == nullptr)
	{
		printf_s("[INFO] CreateIoCompletionPort fail ! \n");
	}


	// worker thread ����
	vector<thread*> worker;
	//worker.reserve(8);
	for (int i = 0; i < 8; ++i)
	{
		worker.push_back(new thread{ &IocpServer::WorkerThread,this });
	}

	// accept thread ����
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

// �������ΰ� 
void IocpServer::WorkerThread()
{


	while (m_bWorkerThread)
	{
		IoData* ioData = nullptr;
		Session* session = nullptr;
		DWORD    size = 0;

		auto ret = GetQueuedCompletionStatus(m_hIOCP,
			&size,							// ������ ���۵� ����Ʈ
			(PULONG_PTR)& session,			// completion key
			(LPOVERLAPPED*)& ioData,		// overlapped I/O ��ü
			INFINITE						// ����� �ð�
		);


		if (!ret)
		{
			continue;
		}

		if (size == 0)
		{
			cout << "[INFO]  ���� ���� session ID : " << session->m_ID << endl;
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

				// ���⼭���� �ٽ� 
			
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
//	// �Լ� ȣ�� ���� ����
//	BOOL	bResult;
//	int		nResult;
//	// Overlapped I/O �۾����� ���۵� ������ ũ��
//	DWORD	recvBytes;
//	DWORD	sendBytes;
//	// Completion Key�� ���� ������ ����
//	stSOCKETINFO* pCompletionKey;
//	// I/O �۾��� ���� ��û�� Overlapped ����ü�� ���� ������	
//	stSOCKETINFO* pSocketInfo;
//	// 
//	DWORD	dwFlags = 0;
//
//	while (m_bWorkerThread)
//	{
//		
//		/**
//		 * �� �Լ��� ���� ��������� WaitingThread Queue �� �����·� ���� ��
//		 * �Ϸ�� Overlapped I/O �۾��� �߻��ϸ� IOCP Queue ���� �Ϸ�� �۾��� ������
//		 * ��ó���� ��
//		 */
//		bResult = GetQueuedCompletionStatus(m_hIOCP,
//			&recvBytes,				// ������ ���۵� ����Ʈ
//			(PULONG_PTR)& pCompletionKey,	// completion key
//			(LPOVERLAPPED*)& pSocketInfo,			// overlapped I/O ��ü
//			INFINITE				// ����� �ð�
//		);
//
//		if (!bResult && recvBytes == 0)
//		{
//			printf_s("[INFO] socket(%d) ���� ����\n", pSocketInfo->socket);
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
//			printf_s("[INFO] �޽��� ����- Bytes : [%d], Msg : [%s]\n",
//				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);
//
//			// Ŭ���̾�Ʈ�� ������ �״�� �۽�			
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
//				printf_s("[ERROR] WSASend ���� : ", WSAGetLastError());
//			}
//
//			printf_s("[INFO] �޽��� �۽� - Bytes : [%d], Msg : [%s]\n",
//				pSocketInfo->dataBuf.len, pSocketInfo->dataBuf.buf);
//
//			// stSOCKETINFO ������ �ʱ�ȭ
//			ZeroMemory(&(pSocketInfo->overlapped), sizeof(OVERLAPPED));
//			pSocketInfo->dataBuf.len = MAX_BUFFER;
//			pSocketInfo->dataBuf.buf = pSocketInfo->messageBuffer;
//			ZeroMemory(pSocketInfo->messageBuffer, MAX_BUFFER);
//			pSocketInfo->recvBytes = 0;
//			pSocketInfo->sendBytes = 0;
//
//			dwFlags = 0;
//
//			// Ŭ���̾�Ʈ�κ��� �ٽ� ������ �ޱ� ���� WSARecv �� ȣ������
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
//				printf_s("[ERROR] WSARecv ���� : ", WSAGetLastError());
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
	// Ŭ���̾�Ʈ ����
	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET acceptedSocket;



	printf_s("[INFO] Accepting...\n");

	// Ŭ���̾�Ʈ ������ ����
	while (m_bAccept)
	{
		acceptedSocket = WSAAccept(
			m_listenSocket, (struct sockaddr*) & clientAddr, &addrLen, NULL, NULL
		);

		if (acceptedSocket == INVALID_SOCKET)
		{
			printf_s("[ERROR] Accept ����\n");
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

