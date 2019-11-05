#include "ChatServer.h"
#include <string>
#include <iostream>

const unsigned int  SERVER_PORT = 8000;
const unsigned int  MAX_CLIENT = 100;		//�� �����Ҽ� �ִ� Ŭ���̾�Ʈ ��

int main()
{
	ChatServer server;

	//������ �ʱ�ȭ
	server.InitSocket();
	server.BindandListen(SERVER_PORT);

	// start 
	server.Run(MAX_CLIENT);

	printf("�ƹ� Ű�� ���� ������ ����մϴ�\n");
	getchar();

	server.DestroyThread();
	return 0;
}

