#pragma once
#pragma warning( disable:4786 )
#pragma warning( disable:4251 )
#pragma warning( disable:4311 )
#pragma warning( disable:4244 )
#pragma warning( disable:4996 )

// 서버 관련 디파인들 정의
#include "ServerOpt.h"


#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <Mswsock.h>
#include <windows.h>



#include <vector>
#include <deque>
#include <array>
#include <list>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <stdio.h>
#include <memory>
#include <iostream>


const int SERVERPORT{ 9000 };
//const char* IP{ "127.0.0.1" };
const int CPU_CORE = 4;


const int SOCKET_BUF_SIZE = 1024;
const int MAX_IO_SIZE = 1024;

const int MAX_CLIENTS = 1000;