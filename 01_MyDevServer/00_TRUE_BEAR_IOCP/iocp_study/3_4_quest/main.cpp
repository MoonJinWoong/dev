#include "ChatServer.h"
#include <string>
#include <iostream>

const unsigned int  SERVER_PORT = 8000;
const unsigned int  MAX_CLIENT = 100;		//총 접속할수 있는 클라이언트 수

int main()
{
	ChatServer server;

	//소켓을 초기화
	server.InitSocket();
	server.BindandListen(SERVER_PORT);

	// start 
	server.Run(MAX_CLIENT);

	printf("show me the money...\n");
	getchar();

	server.DestroyThread();
	return 0;
}

