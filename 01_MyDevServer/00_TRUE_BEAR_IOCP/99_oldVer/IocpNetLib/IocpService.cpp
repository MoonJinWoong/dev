#include "IocpService.h"


// 글로벌하게 사용
IocpService* gIocpService = nullptr;


char IocpService::mAcceptBuf[64] = { 0, };
LPFN_ACCEPTEX IocpService::mlpfnAcceptEx = nullptr;


BOOL AcceptEx(SOCKET sListenSocket, SOCKET sAcceptSocket, PVOID lpOutputBuffer, DWORD dwReceiveDataLength,
	DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength, LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped)
{
	return IocpService::mlpfnAcceptEx(sListenSocket, sAcceptSocket, lpOutputBuffer, dwReceiveDataLength,
		dwLocalAddressLength, dwRemoteAddressLength, lpdwBytesReceived, lpOverlapped);
}

IocpService::IocpService()
{
	mCompletionPort = NULL;
	mListenSocket = NULL;
	memset(mIoWorkerThread, 0, sizeof(mIoWorkerThread));
}

IocpService::~IocpService()
{
}

bool IocpService::Init()
{
	/// winsock initializing
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf_s("[DEBUG] WSAStartup error: %d\n", WSAGetLastError());
		return false;
	}


	/// Create I/O Completion Port
	mCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (mCompletionPort == INVALID_HANDLE_VALUE)
	{
		printf_s("[DEBUG] CreateIoCompletionPort error: %d\n", WSAGetLastError());
		return false;
	}



	/// create TCP socket
	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mListenSocket == INVALID_SOCKET)
	{
		printf_s("[DEBUG] create TCP socket error: %d\n", WSAGetLastError());
		return false;
	}
	HANDLE handle = CreateIoCompletionPort((HANDLE)mListenSocket, mCompletionPort, 0, 0);
	if (handle != mCompletionPort)
	{
		printf_s("[DEBUG] listen socket IOCP register error: %d\n", GetLastError());
		return false;
	}

	// 재사용할 것
	int opt = 1;
	setsockopt(mListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)& opt, sizeof(int));

	/// bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(mListenSocket, (SOCKADDR*)& serveraddr, sizeof(serveraddr)))
	{
		printf_s("[DEBUG] listen socket bind error: %d\n", GetLastError());
		return false;
	}


	// AcceptEX 를 메모리에 등록
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	DWORD bytes = 0;
	if (SOCKET_ERROR == WSAIoctl(
		mListenSocket, 
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx,
		sizeof(GUID), 
		&mlpfnAcceptEx,     // 이 함수 포인터로 AcceptEX 사용해야함. 헷갈림 주의...
		sizeof(LPFN_ACCEPTEX), 
		&bytes, 
		NULL, 
		NULL)
	) 
	{
		return false;
	}


	/// 세션풀을 만들자.
	gSessionMgr->CreateSessionPool();

	printf_s("[DEBUG][%s] Success\n", __FUNCTION__);
	return true;
}


void IocpService::RunAsyncAccept()
{
	/// listen
	if (SOCKET_ERROR == listen(mListenSocket, SOMAXCONN))
	{
		printf_s("[DEBUG] listen error\n");
		return;
	}

	printf_s("[DEBUG][%s]\n", __FUNCTION__);

	while (gSessionMgr->AcceptClientSessions())
	{
		Sleep(32);
	}

	printf_s("[DEBUG][%s] End Thread\n", __FUNCTION__);
}

void IocpService::ShutDownService()
{
	for (int i = 0; i < MAX_IO_THREAD; ++i)
	{
		CloseHandle(mIoWorkerThread[i]->GetHandle());
	}

	CloseHandle(mCompletionPort);

	/// winsock finalizing
	WSACleanup();

}






bool IocpService::CreateThreads()
{
	/// create I/O Thread
	for (int i = 0; i < MAX_IO_THREAD; ++i)
	{
		unsigned __int64 threadIndex = i;
		DWORD dwThreadId = 0;

		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, IoWorkerThread, (PVOID)threadIndex, CREATE_SUSPENDED, (unsigned int*)& dwThreadId);
		if (hThread == NULL) 
		{
			return false;
		}

		mIoWorkerThread[i] = new IocpThread(hThread, mCompletionPort);
		mIoWorkerThread[i]->SetManagedSendIOClientSessionIndex(gSessionMgr->getMaxSessionCount(), i, MAX_IO_THREAD);
	}

	/// start!
	for (int i = 0; i < MAX_IO_THREAD; ++i)
	{
		ResumeThread(mIoWorkerThread[i]->GetHandle());
	}

	return true;
}

unsigned int WINAPI IocpService::IoWorkerThread(LPVOID lpParam)
{
	auto LWorkerThreadId = reinterpret_cast<unsigned __int64>(lpParam);

	printf_s("[DEBUG][%s] LWorkerThreadId: %I64u\n", __FUNCTION__, LWorkerThreadId);

	return gIocpService->mIoWorkerThread[LWorkerThreadId]->Run();
}



