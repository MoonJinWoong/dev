/***********************************************

		스레드를사용한채팅클라이언트

   원래루트는recv 전용이고

   스레드를생성해서사용자입력대기하다가,

   입력받게되면send 해주는클라이언트임.

***********************************************/

#pragma warning(disable : 4996)

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdlib.h>

#include <stdio.h>

#include <string.h>



#define MAXLINE     512



DWORD WINAPI ProcessInputSend(LPVOID arg);



const char *escapechar = "exit";

char name[10]; // 채팅에서사용할이름

char line[MAXLINE], message[MAXLINE + 1];

struct    sockaddr_in   server_addr;

SOCKET s;      // 서버와연결된소켓번호



int main()

{

	WSADATA wsa;                          // 윈속



	// 채팅참가자이름구조체초기화

	printf("채팅ID 입력: ");

	scanf("%s", name);



	// 윈속초기화

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)

		exit(1);



	// 소켓생성

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)

	{

		printf("Client : Can't open stream socket.\n");

		exit(0);

	}



	// 채팅서버의소켓주소구조체server_addr 초기화

	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;

	server_addr.sin_port = htons(9000);

	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");



	// 연결요청

	if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)

	{

		printf("Client : Can't connect to server.\n");

		exit(0);

	}

	else

	{

		printf("서버에접속되었습니다. \n");

	}



	// 스레드생성

	HANDLE hThread;

	DWORD ThreadId;

	hThread = CreateThread(NULL, 0, ProcessInputSend, 0, 0, &ThreadId);

	if (hThread == NULL)

		printf("[오류] 스레드생성실패!\n");

	else

		CloseHandle(hThread);



	while (1)

	{

		ZeroMemory(message, sizeof(message));

		int size;

		if ((size = recv(s, message, MAXLINE, 0)) == SOCKET_ERROR)

		{

			printf("recv()");

			exit(0);

		}

		else

		{

			message[size] = '\0';

			printf("%s \n", message);

		}

	}

	closesocket(s);                       // closesocket()

	WSACleanup();                 // 윈속종료

}





// 사용자입력부분

DWORD WINAPI ProcessInputSend(LPVOID arg)

{

	while (true)

	{

		if (fgets(message, MAXLINE, stdin))

		{

			sprintf(line, "[%s] %s", name, message);

			if (send(s, line, strlen(line), 0) < 0)

				printf("Error : Write error on socket.\n");

			if (strstr(message, escapechar) != NULL)

			{

				printf("Good bye.\n");

				closesocket(s);

				exit(0);

			}

		}

	}

	return 0;
}