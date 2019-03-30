#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����


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

unsigned WINAPI SendMsg(void* arg);//������ �����Լ�
unsigned WINAPI RecvMsg(void* arg);//������ �����Լ�
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



	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)// ������ ������ ����Ѵٰ� �ü���� �˸�
		printf("WSAStartup() error!");

	//sprintf(name, "[%s]", argv[3]);
	sock = socket(PF_INET, SOCK_STREAM, 0);//������ �ϳ� �����Ѵ�.

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(atoi("9000"));

	if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)//������ �����Ѵ�.
		printf("connect() error");

	//���ӿ� �����ϸ� �� �� �Ʒ��� ����ȴ�.

	sendThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&sock, 0, NULL);//�޽��� ���ۿ� �����尡 ����ȴ�.
	recvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&sock, 0, NULL);//�޽��� ���ſ� �����尡 ����ȴ�.

	WaitForSingleObject(sendThread, INFINITE);//���ۿ� �����尡 �����ɶ����� ��ٸ���./
	WaitForSingleObject(recvThread, INFINITE);//���ſ� �����尡 �����ɶ����� ��ٸ���.
	//Ŭ���̾�Ʈ�� ���Ḧ �õ��Ѵٸ� ���� �Ʒ��� ����ȴ�.
	closesocket(sock);//������ �����Ѵ�.
	WSACleanup();//������ ���� ��������� �ü���� �˸���.
	return 0;
}

unsigned WINAPI SendMsg(void* arg) {//���ۿ� �������Լ�
	SOCKET sock = *((SOCKET*)arg);//������ ������ �����Ѵ�.
	char nameMsg[BUF_SIZE];
	while (1) {//�ݺ�
		//ZeroMemory(nameMsg, sizeof(nameMsg));
		fgets(msg, BUF_SIZE, stdin);//�Է��� �޴´�.
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {//q�� �Է��ϸ� �����Ѵ�.
			closesocket(sock);
			exit(0);
		}
		sprintf(nameMsg, "%s", msg);//nameMsg�� �޽����� �����Ѵ�.
		
		
		lockObj.lock();
		send(sock, nameMsg, strlen(nameMsg), 0);//nameMsg�� �������� �����Ѵ�.
		printf("cli -> server ... send call\n");
		lockObj.unlock();
	}
	return 0;
}

unsigned WINAPI RecvMsg(void* arg) {
	SOCKET sock = *((SOCKET*)arg);//������ ������ �����Ѵ�.
	char nameMsg[BUF_SIZE];
	int strLen= 0;
	while (1) {//�ݺ�
		
		//ZeroMemory(nameMsg, sizeof(nameMsg));
		//lockObj.lock();
		//strLen = recv(sock, nameMsg, BUF_SIZE - 1, 0);//�����κ��� �޽����� �����Ѵ�.
		strLen = recvn(sock, nameMsg, BUF_SIZE - 1, 0);
		//lockObj.unlock();
		if (strLen == -1)
			return -1;
		printf("���� �޼��� - %s",nameMsg);
		nameMsg[strLen] = 0;//���ڿ��� ���� �˸��� ���� ����
		
		//fputs(nameMsg, stdout);//�ڽ��� �ֿܼ� ���� �޽����� ����Ѵ�.
		
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


