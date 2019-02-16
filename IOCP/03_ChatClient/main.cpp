#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>

using namespace std;

DWORD WINAPI SendThread(LPVOID lpData);

char ID[10];



int main()
{
	std::cout << "Hello" << std::endl;

	WSADATA		WsaData;
	int nRet = WSAStartup(MAKEWORD(2, 2), &WsaData);

	auto local_ip = "127.0.0.1";
	auto port = "32452";
	
	// ���� ����
	SOCKET socket_client = socket(AF_INET, SOCK_STREAM, 0);



	// ���� �ּ� ����
	SOCKADDR_IN servAddr = { 0 };
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = inet_addr(local_ip);
		servAddr.sin_port = htons(atoi(port)); // ��Ʈ

		// ���� ����
		if (connect(socket_client, (struct sockaddr *) &servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		{
			closesocket(socket_client);
			printf("������ ���� �Ҽ� �����ϴ�.");
			return SOCKET_ERROR;
		}

		CreateThread(NULL, 0, SendThread, (LPVOID)socket_client, 0, NULL);
		// ���� ���
		DWORD dwTmp;
		char text[1024] = "������ ���ӵǾ����ϴ�.\n";
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, strlen(text), &dwTmp, NULL);

		// ���� & ���� ��Ǫ
		while (1)
		{
			char buffer[1024] = { 0 };
			int len = recv(socket_client, buffer, sizeof(buffer), 0);
			if (len <= 0)
			{
				printf("���� ����.");
				break;
			}

			// �޽��� ���
			WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), &dwTmp, NULL);
		}
		//���� ����
		closesocket(socket_client);
		return 0;
	}


	DWORD WINAPI SendThread(LPVOID lpData)
	{
		SOCKET socket_client = (SOCKET)lpData;
		while (1)
		{
			printf("������ ���� : ");
			DWORD dwTmp;
			char text[1024] = { 0 };
			ReadFile(GetStdHandle(STD_INPUT_HANDLE), text, 1024, &dwTmp, NULL);
			send(socket_client, text, strlen(text), 0);
		}
		return 0;
	}

