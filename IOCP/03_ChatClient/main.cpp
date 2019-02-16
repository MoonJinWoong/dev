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
	
	// 소켓 생성
	SOCKET socket_client = socket(AF_INET, SOCK_STREAM, 0);



	// 소켓 주소 정보
	SOCKADDR_IN servAddr = { 0 };
		servAddr.sin_family = AF_INET;
		servAddr.sin_addr.s_addr = inet_addr(local_ip);
		servAddr.sin_port = htons(atoi(port)); // 포트

		// 소켓 접속
		if (connect(socket_client, (struct sockaddr *) &servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		{
			closesocket(socket_client);
			printf("서버에 접속 할수 없습니다.");
			return SOCKET_ERROR;
		}

		CreateThread(NULL, 0, SendThread, (LPVOID)socket_client, 0, NULL);
		// 상태 출력
		DWORD dwTmp;
		char text[1024] = "서버에 접속되었습니다.\n";
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), text, strlen(text), &dwTmp, NULL);

		// 전송 & 수신 루푸
		while (1)
		{
			char buffer[1024] = { 0 };
			int len = recv(socket_client, buffer, sizeof(buffer), 0);
			if (len <= 0)
			{
				printf("접속 종료.");
				break;
			}

			// 메시지 출력
			WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), &dwTmp, NULL);
		}
		//소켓 해제
		closesocket(socket_client);
		return 0;
	}


	DWORD WINAPI SendThread(LPVOID lpData)
	{
		SOCKET socket_client = (SOCKET)lpData;
		while (1)
		{
			printf("전송할 내용 : ");
			DWORD dwTmp;
			char text[1024] = { 0 };
			ReadFile(GetStdHandle(STD_INPUT_HANDLE), text, 1024, &dwTmp, NULL);
			send(socket_client, text, strlen(text), 0);
		}
		return 0;
	}

