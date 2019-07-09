#include "PreCompile.h"


using namespace std;
const int SOCKET_BUF_SIZE = 1024*10;

typedef enum {
	IO_READ,
	IO_WRITE,
	
}IO_OP;

struct SessionData
{
	OVERLAPPED  s_over;
	WSABUF		s_wsabuf;
	IO_OP		s_ioType;

	int			s_totalByte;
	int			s_currByte;
	char		s_buffer[SOCKET_BUF_SIZE];
};

struct SocketData
{
	SOCKET			s_sock;
	SOCKADDR_IN		s_addrInfo;
	char			s_ipAddr[16];
	SessionData		s_sessionData;
};


DWORD WINAPI AcceptThread(LPVOID context);
DWORD WINAPI WorkerThread(LPVOID context);

SOCKET listenSocket;
const int WORKER_THREAD_NUMBER = 5;


int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cout << "WSASatrtup is fail..!" << std::endl;
		return 1;
	}

	HANDLE iocp;

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, WORKER_THREAD_NUMBER);
	if (!iocp)
	{
		std::cout << "CreateIoCompletionPort is fail..!" << std::endl;
		return 1;
	}

	// worker 시작

	for (int i = 0; i < WORKER_THREAD_NUMBER; i++)
	{
		HANDLE thread = CreateThread(NULL, 0, WorkerThread, iocp, 0, 0);
		CloseHandle(thread);
	}

	// 여기서부터 시작 컴파일 오류 난다 워커스레드 함수 만들어주어야함 
	// 교보책 참고


	return 0;
}

