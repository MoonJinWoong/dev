#include "preCompile.h"





int main()
{

	// ���� ����
	INITCONFIG initconfig;
	initconfig.sServerPort = 32452;
	initconfig.sRecvBufCnt = 10;
	initconfig.sRecvBufSize = 1024;
	initconfig.sProcessPacketCnt = 1000;
	initconfig.sSendBufCnt = 10;
	initconfig.sSendBufSize = 1024;
	initconfig.sWorkerThreadCnt = 2;
	initconfig.sProcessThreadCnt = 1;





	// ��ü ���� �� �ʱ�ȭ
	IocpServer* chatserver = new IocpServer();
	if (chatserver->start(initconfig))
		cout << "********** server init success **********" << endl;






	// accept thread ����� ������.
	chatserver->makeThreads();


	return 0;
}