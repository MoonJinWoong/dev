#include "NetBase.h"

NetBase::NetBase()
{
	// �����ڿ��� content�� �޾Ƽ� ������ �����ؾ���. �̰�
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

	// ���� �ɼ� 
	strcpy_s(mIP, "127.0.0.1");
	mPort = 9000;

	//// ������ �۾� �������� ������ (CPU * 2) + 1
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

