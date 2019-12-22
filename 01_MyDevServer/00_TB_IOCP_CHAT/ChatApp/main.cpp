#include "ChatServer.h"
#include <string>
#include <iostream>

const unsigned int  MAX_CLIENT = 100;		//�� �����Ҽ� �ִ� Ŭ���̾�Ʈ ��

int main()
{


	ChatServer server;
	server.InitSocket();
	server.BindandListen();

	// start 
	server.Run();

	LOG(INFO) << "****** SERVER START ******* " << std::endl;
	getchar();

	server.Stop();
	LOG(INFO) << "****** SERVER STOP ******* " << std::endl;
	return 0;
}


