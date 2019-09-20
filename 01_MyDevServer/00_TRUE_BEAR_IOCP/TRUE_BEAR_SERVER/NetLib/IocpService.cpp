#include "IocpService.h"



IocpService::IocpService() {}
IocpService::~IocpService() {}

void IocpService::Ready()
{

	mIocp.InitIocp(8);
	mNetObject.MakeSocket();
	
	if (mNetObject.BindAndListen(EndAddress("0.0.0.0", 8000)))
	{
		std::cout << "Net Obj BindAndListen fail.." << std::endl;
	}

	mIocp.ResisterIocp(mNetObject, nullptr);
}