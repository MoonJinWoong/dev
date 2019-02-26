#pragma once

#pragma warning( disable:4786 )
#pragma warning( disable:4251 )
#pragma warning( disable:4311 )
#pragma warning( disable:4244 )
#pragma warning( disable:4996 )

#ifndef DLLTEST_EXPORTS
#define DLLTEST_API __declspec(dllexport)
#else
#define DLLTEST_API __declspec(dllimport)
#endif // !DLLTEST_EXPORTS


#include <iostream>
#include <mutex>




#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mswsock.lib")  




#include "IocpServer.h"
#include "test.h"




using namespace std;