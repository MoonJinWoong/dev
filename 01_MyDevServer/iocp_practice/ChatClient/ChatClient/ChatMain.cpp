/***********************************************

		�����带�����ä��Ŭ���̾�Ʈ

   ������Ʈ��recv �����̰�

   �����带�����ؼ�������Է´���ϴٰ�,

   �Է¹ްԵǸ�send ���ִ�Ŭ���̾�Ʈ��.

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

char name[10]; // ä�ÿ���������̸�

char line[MAXLINE], message[MAXLINE + 1];

struct    sockaddr_in   server_addr;

SOCKET s;      // �����Ϳ���ȼ��Ϲ�ȣ



int main()

{

	WSADATA wsa;                          // ����



	// ä���������̸�����ü�ʱ�ȭ

	printf("ä��ID �Է�: ");

	scanf("%s", name);



	// �����ʱ�ȭ

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)

		exit(1);



	// ���ϻ���

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)

	{

		printf("Client : Can't open stream socket.\n");

		exit(0);

	}



	// ä�ü����Ǽ����ּұ���üserver_addr �ʱ�ȭ

	ZeroMemory(&server_addr, sizeof(server_addr));

	server_addr.sin_family = AF_INET;

	server_addr.sin_port = htons(9000);

	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");



	// �����û

	if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)

	{

		printf("Client : Can't connect to server.\n");

		exit(0);

	}

	else

	{

		printf("���������ӵǾ����ϴ�. \n");

	}



	// ���������

	HANDLE hThread;

	DWORD ThreadId;

	hThread = CreateThread(NULL, 0, ProcessInputSend, 0, 0, &ThreadId);

	if (hThread == NULL)

		printf("[����] �������������!\n");

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

	WSACleanup();                 // ��������

}





// ������Էºκ�

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