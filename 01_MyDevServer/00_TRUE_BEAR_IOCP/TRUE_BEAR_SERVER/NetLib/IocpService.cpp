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


	// todo 
	// packet mgr , user mgr , room mgr ±¸Çö


}

bool IocpService::ProcessMsg(OUT INT8& msgType , OUT INT32& sessionIdx , 
					char* buf,OUT INT16& copySize , const INT32 wait)
{
	//Message* pMsg = nullptr;
	//Connection* pConnection = nullptr;
	DWORD ioSize = 0;
	auto waitTime = wait;

	if (waitTime == 0)
	{
		waitTime = INFINITE;
	}







	return true;
}



