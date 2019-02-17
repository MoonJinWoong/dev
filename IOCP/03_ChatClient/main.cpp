#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <conio.h>
#include <thread>
#include <process.h>
#include <iostream>

#include "overlapped.h"

#define BUF_SIZE 256
#define NICK_LEN 32
#define LINE_NUM 32

#define COL  100
#define LINE (LINE_NUM + 1)


#pragma warning(disable : 4996)

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGSasd


#pragma comment(lib, "ws2_32.lib")
#define MAX_BUFFER        1024
#define SERVER_IP        "127.0.0.1"
#define SERVER_PORT        32452



using namespace std;

enum {PACKET_CHAT = 1};

SOCKET listenSocket;
int main()
{
	// Winsock Start - winsock.dll 로드
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
	{
		printf("Error - Can not load 'winsock.dll' file\n");
		return 1;
	}

	// 1. 소켓생성
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);

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

	while (1)
	{
		// 메시지 입력
		char messageBuffer[MAX_BUFFER];
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

		Packet_Chat Chat;
		Chat.s_nLength = sizeof(Packet_Chat);
		Chat.s_sType = PACKET_CHAT;
		strcpy(Chat.s_szIP, "127.0.0.1");
		strncpy(Chat.s_szChatMsg, messageBuffer, bufferLen);
		Chat.s_szChatMsg[bufferLen] = NULL;
		send(listenSocket, (char*)&Chat, sizeof(Packet_Chat), 0);


		char recvmessageBuffer[MAX_BUFFER];
		Packet_Chat recvChat;
		recvChat.s_nLength = sizeof(Packet_Chat);
		recvChat.s_sType = PACKET_CHAT;
		strcpy(recvChat.s_szIP, "127.0.0.1");
		strncpy(recvChat.s_szChatMsg, recvmessageBuffer, bufferLen);
		int nRcv = recv(listenSocket, (char*)&recvChat, sizeof(Packet_Chat) - 1, 0);

		cout << recvChat.s_szChatMsg << endl;
		//recvChat.s_szChatMsg[bufferLen] = NULL;
	}

		


	

	// 4. 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return 0;
}




