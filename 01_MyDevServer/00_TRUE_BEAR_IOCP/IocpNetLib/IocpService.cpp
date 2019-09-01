#include "IocpService.h"


// 글로벌하게 사용
IocpService* gIocpService = nullptr;
LPFN_ACCEPTEX IocpService::mFnAcceptEx = nullptr;
LPFN_CONNECTEX IocpService::mFnConnectEx = nullptr;
char IocpService::mAcceptBuf[64] = { 0, };


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

	/*GUID guidDisconnectEx = WSAID_DISCONNECTEX ;
	DWORD bytes = 0 ;
	if (SOCKET_ERROR == WSAIoctl(mListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidDisconnectEx, sizeof(GUID), &mFnDisconnectEx, sizeof(LPFN_DISCONNECTEX), &bytes, NULL, NULL) )
		return false;
*/
	DWORD bytes = 0;

	GUID guidAcceptEx = WSAID_ACCEPTEX;
	if (SOCKET_ERROR == WSAIoctl(mListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx, sizeof(GUID), &mFnAcceptEx, sizeof(LPFN_ACCEPTEX), &bytes, NULL, NULL)) {
		return false;
	}

	GUID guidConnectEx = WSAID_CONNECTEX;
	if (SOCKET_ERROR == WSAIoctl(mListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidConnectEx, sizeof(GUID), &mFnConnectEx, sizeof(LPFN_CONNECTEX), &bytes, NULL, NULL)) {
		return false;
	}

	/// make session pool
	gSessionMgr->PrepareClientSessions();

	printf_s("[DEBUG][%s] Success\n", __FUNCTION__);
	return true;
}



