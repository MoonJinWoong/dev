#include "stdafx.h"
#include "..//01_NetLibrary/preComplie.h"
#include <stdio.h>
#include <signal.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <memory>
#include <vector>
#include <iostream>
#include <mutex>
#include "IocpService.h"

using namespace std;
volatile bool stopServer = false;


int main()
{	
	IocpService* iocpservice = new IocpService();
	iocpservice->ReadyIocp();
	iocpservice->CreateThreads();
		
	
	return 0;
}

