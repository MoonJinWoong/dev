#pragma once

// �����Ͽ� �ʿ��� �����
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>
#include <process.h>
#include <memory>
#include <vector>
#include <chrono>  //�ð� ������
#include <mutex>

using namespace std;
using namespace chrono;

// �������� define ���� �͵�
#include "Common.h"

#include "Iocp.h"
#include "Connection.h"