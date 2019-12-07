#include "ChatServer.h"
#include <string>
#include <iostream>

const unsigned int  MAX_CLIENT = 100;		//�� �����Ҽ� �ִ� Ŭ���̾�Ʈ ��

int main()
{
	ChatServer server;
	server.InitConfig();
	server.InitSocket();
	server.BindandListen();

	// start 
	server.Run(MAX_CLIENT);

	printf("show me the money...\n");
	getchar();

	server.Stop();
	return 0;
}

