#include "preCompile.h"





int main()
{

	// 서버 정보
	INITCONFIG initconfig;
	initconfig.sServerPort = 32452;
	initconfig.sRecvBufCnt = 10;
	initconfig.sRecvBufSize = 1024;
	initconfig.sProcessPacketCnt = 1000;
	initconfig.sSendBufCnt = 10;
	initconfig.sSendBufSize = 1024;
	initconfig.sWorkerThreadCnt = 2;
	initconfig.sProcessThreadCnt = 1;





	// 객체 생성 및 초기화
	IocpServer* chatserver = new IocpServer();
	if (chatserver->start(initconfig))
		cout << "********** server init success **********" << endl;






	// accept thread 만들고 돌린다.
	chatserver->makeThreads();


	return 0;
}