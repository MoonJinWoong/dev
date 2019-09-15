#pragma once 
#pragma comment(lib, "winmm.lib") // timeBegin,end Period()  사용을 위해

#include <string>
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <memory>
#include <process.h>
#include <iostream>
#include <list>
#include <vector>

const int MAX_IO_THREAD = 4;
const int PORT = 8000;


#include "IocpThread.h"
#include "FastSpinLock.h"
#include "IocpService.h"
#include "ExOverlappedIO.h"
#include "Session.h"
#include "SessionManager.h"


