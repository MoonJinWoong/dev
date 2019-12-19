#include "ChatServer.h"
#include <string>
#include <iostream>

const unsigned int  MAX_CLIENT = 100;		//총 접속할수 있는 클라이언트 수

int main()
{


	ChatServer server;
	server.InitSocket();
	server.BindandListen();

	// start 
	server.Run();

	printf("show me the money...\n");
	getchar();

	server.Stop();
	return 0;
}


