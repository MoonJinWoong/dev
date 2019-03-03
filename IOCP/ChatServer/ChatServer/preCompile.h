#pragma once

//이곳에서 통합관리


#pragma warning( disable:4786 )
#pragma warning( disable:4251 )
#pragma warning( disable:4311 )
#pragma warning( disable:4244 )
#pragma warning( disable:4996 )

#pragma comment(lib,"Ws2_32.lib")


#include <iostream>
#include <WinSock2.h>
#include <winsock.h>
#include <thread>
#include <vector>


using namespace std;


#include "protocol.h"
#include "IocpServer.h"
#include "ConnectionManager.h"

