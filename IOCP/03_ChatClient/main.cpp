#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <conio.h>
#include <thread>
#include <process.h>


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

enum {PACKET_CHAT = 1};

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
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
		//m_AsyncSocket.SendMsg((char*)&Chat, sizeof(Packet_Chat));
		send(listenSocket, (char*)&Chat, sizeof(Packet_Chat),0);
		//// 3-1. 데이터 쓰기
		//int sendBytes = send(listenSocket, messageBuffer, bufferLen, 0);
		//if (sendBytes > 0)
		//{
		//	printf("TRACE - Send message : %s (%d bytes)\n", messageBuffer, sendBytes);
		//	// 3-2. 데이터 읽기
		//	int receiveBytes = recv(listenSocket, messageBuffer, MAX_BUFFER, 0);
		//	if (receiveBytes > 0)
		//	{
		//		printf("TRACE - Receive message : %s (%d bytes)\n* Enter Message\n->", messageBuffer, receiveBytes);
		//	}
		//}

	}

	// 4. 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return 0;
}




