//#include "ConnectionManager.h"
//#include "IOCPChatServer.cpp"

#include "IOCPChatServer.h"


IOCPChatServer* chatserver = new IOCPChatServer();


void Init()
{

	INITCONFIG InitConfig;
	InitConfig.nServerPort = 32452;
	InitConfig.nRecvBufCnt = 10;
	InitConfig.nRecvBufSize = 1024;
	InitConfig.nProcessPacketCnt = 1000;
	InitConfig.nSendBufCnt = 10;
	InitConfig.nSendBufSize = 1024;
	InitConfig.nWorkerThreadCnt = 2;
	InitConfig.nProcessThreadCnt = 1;


	//chatserver()->

	// ServerStart()���� �������� �ʱ�ȭ ����
	if (true == chatserver->ServerStart(InitConfig))
		cout << "char server object create success ... !!! " << endl;
	
	if (true == g_ConnectionManager()->CreateConnection(InitConfig, 10))
		cout << "connection object create success ... !!!! " << endl;

}


void End()
{
	//CLOSE_LOG();
	Singleton::releaseAll();
}

int main()
{
	Init();
	//BindAndListen();
	

	std::cout << "Ű�� ������ ����..." << std::endl;
	getchar();

	End();

	return 0;
}