#include "ChatServer.h"
#include <string>
#include <iostream>

const unsigned int  SERVER_PORT = 8000;
const unsigned int  MAX_CLIENT = 3;		//총 접속할수 있는 클라이언트 수

int main()
{
	ChatServer server;

	server.InitSocket();
	server.BindandListen(SERVER_PORT);

	// start 
	server.Run(MAX_CLIENT);

	printf("show me the money...\n\n\n");
	getchar();

	server.Stop();
	return 0;
}

