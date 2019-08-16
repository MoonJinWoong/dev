#include "NetBase.h"

NetBase::NetBase()
{
	// 생성자에서 content를 받아서 서버를 규정해야함. 이거
	// mContent = param
	mServerStatus = STOP;


}

NetBase::~NetBase()
{
	// shutDown();
	mServerStatus = STOP;

}

void NetBase::InitBase()
{

	// 서버 옵션 
	strcpy_s(mIP, "127.0.0.1");
	mPort = 9000;

	//// 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	mWorkerThreadCnt = sysInfo.dwNumberOfProcessors * 2;
	
	mServerStatus = INIT;

}

void NetBase::PutPackage()
{
	// param : Package *package

	// mContent->putPackage(package);
}

SERVER_STATUS& NetBase::GetServerStatus()
{
	return mServerStatus;
}

