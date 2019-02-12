#include "ConnectionManager.h"


class CChatServer : public IOCPServer
{
	// IOCPServer을(를) 통해 상속됨
	virtual bool OnAccept(Connection * lpConnection) override
	{
		return false;
	}
	virtual bool OnRecv(Connection * lpConnection, DWORD dwSize, char * pRecvedMsg) override
	{
		return false;
	}
	virtual bool OnRecvImmediately(Connection * lpConnection, DWORD dwSize, char * pRecvedMsg) override
	{
		return false;
	}
	virtual void OnClose(Connection * lpConnection) override
	{
	}
	virtual bool OnSystemMsg(Connection * lpConnection, DWORD dwMsgType, LPARAM lParam) override
	{
		return false;
	}
};


void Init()
{
	sLogConfig LogConfig;
	strncpy(LogConfig.s_szLogFileName, "ChatServer", MAX_FILENAME_LENGTH);
	LogConfig.s_nLogInfoTypes[STORAGE_OUTPUTWND] = LOG_ALL;
	LogConfig.s_nLogInfoTypes[STORAGE_WINDOW] = LOG_ALL;
	LogConfig.s_nLogInfoTypes[STORAGE_FILE] = LOG_ERROR_ALL;
	INIT_LOG(LogConfig);

	INITCONFIG InitConfig;
	InitConfig.nServerPort = 32452;
	InitConfig.nRecvBufCnt = 10;
	InitConfig.nRecvBufSize = 1024;
	InitConfig.nProcessPacketCnt = 1000;
	InitConfig.nSendBufCnt = 10;
	InitConfig.nSendBufSize = 1024;
	InitConfig.nWorkerThreadCnt = 2;
	InitConfig.nProcessThreadCnt = 1;


	CChatServer* a = new CChatServer;
	if (a->ServerStart(InitConfig))
	{
		cout << "정상" << endl;
	}

	//a->ServerStart(InitConfig);

	//IocpServer()->ServerStart(InitConfig);

	g_ConnectionManager()->CreateConnection(InitConfig, 10);
	LOG(LOG_INFO_LOW, "서버 시작..");
}

void End()
{
	CLOSE_LOG();
	Singleton::releaseAll();
}

int main()
{
	Init();

	std::cout << "키를 누르면 종료..." << std::endl;
	getchar();

	End();

	return 0;
}