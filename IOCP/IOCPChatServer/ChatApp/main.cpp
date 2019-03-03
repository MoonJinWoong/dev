
//#include "../IOCPChatServer/preCompile.h"
//#pragma comment(lib,"NetLibrary.lib")

#include "Connection.h"
#include "IOCPChatServer.h"



int main()
{
	// server가 시작하기 전에 정의를 하고 config를 넣어준다. 
	INITCONFIG config;
	config.sServerPort = 32452;
	config.sRecvBufCnt = 10;
	config.sRecvBufSize = 1024;
	config.sProcessPacketCnt = 1000;
	config.sSendBufCnt = 10;
	config.sSendBufSize = 1024;
	config.sWorkerThreadCnt = 2;
	config.sProcessThreadCnt = 1;

	IocpServer* obj = new IocpServer();
	
	if (obj->ServerStart(config) == true)
		cout << "server start success ...." << endl;


	Connection* conn = new Connection();
	conn->CreateConnection(config, 1);



	//if (obj->ServerStop() == true)
		//cout << "server stop success...." << endl;
	return 0;
}
