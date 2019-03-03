#pragma once

#pragma warning( disable:4786 )
#pragma warning( disable:4251 )
#pragma warning( disable:4311 )
#pragma warning( disable:4244 )
#pragma warning( disable:4996 )

#ifndef DLLTEST_EXPORTS
#define NETLIB __declspec(dllexport)
#else
#define NETLIB __declspec(dllimport)
#endif // !DLLTEST_EXPORTS


#include <iostream>
#include <mutex>

#include <WinSock2.h>
#include <winsock.h>
#include <mutex>
#include <thread>
#include <process.h>  // beginethreadEx
#include <Mswsock.h>  // acceptEx 
#include <map>				// connection에서 사용
#include <unordered_map>	// 혹시 몰라서 만들어둠
#include <ws2def.h>
#include <functional>


using namespace std;




#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")  





#include "CommonDefine.h"
#include "IocpServer.h"
#include "CircleBuffer.h"
#include "ConnectionManager.h"


// 테스트용
#include "test.h"


