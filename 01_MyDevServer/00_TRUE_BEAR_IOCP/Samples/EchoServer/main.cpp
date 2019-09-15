#include "../IocpNetLib/preCompile.h"



int main()
{
	
	/// Global Managers
	gSessionMgr = new SessionManager;
	gIocpService = new IocpService;


	if (false == gIocpService->Init())
	{
		std::cout << "Fail GIocpManager->Initialize" << std::endl;
		return -1;
	}

	if (false == gIocpService->CreateThreads())
	{
		std::cout << "Fail GIocpManager->Create" << std::endl;
		return -1;
	}

	Sleep(1000);

	std::cout << "Start Server" << std::endl;
	gIocpService->RunAsyncAccept(); 
	gIocpService->ShutDownService();
	std::cout << "End Server" << std::endl;

	delete gSessionMgr;
	delete gIocpService;
	return 0;
}

