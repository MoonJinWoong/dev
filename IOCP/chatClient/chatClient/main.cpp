#include "preCompile.h"

//#pragma comment(lib, "Ws2_32.lib")



WSABUF recvbuf;

int main(void)
{
	// Winsock Start - winsock.dll 로드
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
		return 1;
	}

	// 1. 소켓생성
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Error - Invalid socket\n");
		return 1;
	}

	// 서버정보 객체설정
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	// 2. 연결요청
	if (connect(listenSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Error - Fail to connect\n");
		// 4. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return 1;
	}
	else
	{
		printf("Server Connected\n* Enter Message\n->");
	}


	thread read_thread{ readThread };



	WSAOVERLAPPED_EX *socketInfo;
	DWORD sendBytes;
	DWORD receiveBytes;
	DWORD flags;

	while (1)
	{
		// 메시지 입력
		char messageBuffer[MAX_BUFF_SIZE];
		int i, bufferLen;
		for (i = 0; 1; i++)
		{
			messageBuffer[i] = getchar();
			if (messageBuffer[i] == '\n')
			{
				messageBuffer[i++] = '\0';
				break;
			}
		}
		bufferLen = i;

		socketInfo = (WSAOVERLAPPED_EX *)malloc(sizeof(WSAOVERLAPPED_EX));
		memset((void *)socketInfo, 0x00, sizeof(WSAOVERLAPPED_EX));
		socketInfo->wsabuf.len = bufferLen;
		socketInfo->wsabuf.buf = messageBuffer;
		//socketInfo->event_type = E_SEND;
		DWORD iobyte;
		// 3-1. 데이터 쓰기
		//int sendBytes = send(listenSocket, messageBuffer, bufferLen, 0);

		int ret2 = WSASend(listenSocket, &socketInfo->wsabuf, 1, &iobyte, 0, NULL, NULL);
		printf("send byte -%d\n", iobyte);
		if (iobyte > 0)
		{
			printf("TRACE - Send message : %s (%d bytes)\n", messageBuffer, iobyte);
			
		}

	}

	// 4. 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return 0;
}

void readThread()
{
	while (1)
	{
		
	}
}