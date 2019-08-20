#pragma once


// winsock2 사용을 위해 아래 코멘트 추가
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Mswsock.h> //acceptEx
#include <Ws2tcpip.h>
#include <string>
#include <process.h>
#include <iostream>
#include <thread>
#include <vector>
#include <array>
#include <list>
#include <mutex>
#include <algorithm>
#include <memory>


using namespace std;
#include "NetBase.h"
#include "Session.h"
#include "SessionMgr.h"


const int   SERVER_PORT = 8000;
