#pragma once

// 컴파일에 필요한 헤더들
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <process.h>
#include <memory>
#include <vector>
#include <chrono>  //시간 측정용
#include <mutex>

using namespace std;
using namespace chrono;

// 공통으로 define 해줄 것들
#include "Common.h"

#include "Iocp.h"
#include "Connection.h"