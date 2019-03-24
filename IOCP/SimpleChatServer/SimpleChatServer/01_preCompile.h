

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <map>
#include <vector>
#include <iostream>
#include <thread>
#include <process.h>


using namespace std;

#include "02_Common.h"
#include "04_IocpServer.h"