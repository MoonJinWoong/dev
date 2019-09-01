#include "../IocpNetLib/preCompile.h"



int main()
{

	gSessionMgr = new SessionManager;
	gIocpService = new IocpService;
		
	
	if (!gIocpService->Init())
	{
		std::cout << "IocpService Init failed.....!!" << std::endl;
		return -1;
	}


	delete gSessionMgr;
	delete gIocpService;
	return 0;
}

