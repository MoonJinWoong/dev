#include "ChatProtocol.h"






int main()
{

	// �ʱ�ȭ 
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//���� �ʱ�ȭ


	// socket ����
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//���� ����
	if (sock == -1)
	{
		return -1;
	}


	// �ּ� ���� �� ���� ��û
	SOCKADDR_IN servaddr = { 0 };//���� �ּ�
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(32452);
	int re = 0;
	re = connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));//���� ��û
	if (re == -1)
	{
		return -1;
	}


}