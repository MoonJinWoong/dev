
#include "../IOCPChatServer/preCompile.h"
#pragma comment(lib,"NetLibrary.lib")





int main()
{
	// server�� �����ϱ� ���� ���Ǹ� �ϰ� config�� �־��ش�. 
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





	//if (obj->ServerStop() == true)
		//cout << "server stop success...." << endl;
	return 0;
}
