#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지


#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include <mutex>


#define BUF_SIZE 512
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void* arg);//쓰레드 전송함수
unsigned WINAPI RecvMsg(void* arg);//쓰레드 수신함수
void ErrorHandling(char* msg);
int recvn(SOCKET s, char* buf, int len, int flags);

char name[NAME_SIZE] = "RECV ->";
char msg[BUF_SIZE];

std::mutex lockObj;


int main() {
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN serverAddr;
	HANDLE sendThread, recvThread;



	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)// 윈도우 소켓을 사용한다고 운영체제에 알림
		printf("WSAStartup() error!");

	//sprintf(name, "[%s]", argv[3]);
	sock = socket(PF_INET, SOCK_STREAM, 0);//소켓을 하나 생성한다.

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(atoi("9000"));

	if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)//서버에 접속한다.
		printf("connect() error");

	//접속에 성공하면 이 줄 아래가 실행된다.

	sendThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&sock, 0, NULL);//메시지 전송용 쓰레드가 실행된다.
	recvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&sock, 0, NULL);//메시지 수신용 쓰레드가 실행된다.

	WaitForSingleObject(sendThread, INFINITE);//전송용 쓰레드가 중지될때까지 기다린다./
	WaitForSingleObject(recvThread, INFINITE);//수신용 쓰레드가 중지될때까지 기다린다.
	//클라이언트가 종료를 시도한다면 이줄 아래가 실행된다.
	closesocket(sock);//소켓을 종료한다.
	WSACleanup();//윈도우 소켓 사용중지를 운영체제에 알린다.
	return 0;
}

unsigned WINAPI SendMsg(void* arg) {//전송용 쓰레드함수
	SOCKET sock = *((SOCKET*)arg);//서버용 소켓을 전달한다.
	char nameMsg[BUF_SIZE];
	while (1) {//반복
		//ZeroMemory(nameMsg, sizeof(nameMsg));
		fgets(msg, BUF_SIZE, stdin);//입력을 받는다.
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {//q를 입력하면 종료한다.
			closesocket(sock);
			exit(0);
		}
		sprintf(nameMsg, "%s", msg);//nameMsg에 메시지를 전달한다.
		
		
		lockObj.lock();
		send(sock, nameMsg, strlen(nameMsg), 0);//nameMsg를 서버에게 전송한다.
		printf("cli -> server ... send call\n");
		lockObj.unlock();
	}
	return 0;
}

unsigned WINAPI RecvMsg(void* arg) {
	SOCKET sock = *((SOCKET*)arg);//서버용 소켓을 전달한다.
	char nameMsg[BUF_SIZE];
	int strLen= 0;
	while (1) {//반복
		
		//ZeroMemory(nameMsg, sizeof(nameMsg));
		//lockObj.lock();
		//strLen = recv(sock, nameMsg, BUF_SIZE - 1, 0);//서버로부터 메시지를 수신한다.
		strLen = recvn(sock, nameMsg, BUF_SIZE - 1, 0);
		//lockObj.unlock();
		if (strLen == -1)
			return -1;
		printf("받은 메세지 - %s",nameMsg);
		nameMsg[strLen] = 0;//문자열의 끝을 알리기 위해 설정
		
		//fputs(nameMsg, stdout);//자신의 콘솔에 받은 메시지를 출력한다.
		
	}
	return 0;
}

void ErrorHandling(char* msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
	int rcv;
	char* ptr = buf;
	int left = len;
	while (left > 0)
	{
		rcv = recv(s, ptr, left, flags);
		if (rcv == SOCKET_ERROR)

		{
			return SOCKET_ERROR;
		}
		else if (rcv == 0)
		{
			break;
		}
		left -= rcv;
		ptr += rcv;
	}

	return (len - left);

}


