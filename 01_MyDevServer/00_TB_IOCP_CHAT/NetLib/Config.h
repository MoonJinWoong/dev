#pragma once
#include <Windows.h>
#include <atlstr.h>
#include <string>



namespace Config
{
	static int LoadNetConfig(const wchar_t* category , const wchar_t* optName)
	{
		wchar_t buf[100] = { 0, };
		auto result = GetPrivateProfileString(category, optName, L"-1", buf, 100, L"..\\config.ini");
		std::wstring ResultString(buf, result);

		return std::stoi(ResultString);
	}
}


// config category
const wchar_t CATEGORY_NET[] = L"NETWORK";
const wchar_t CATEGORY_LOGIC[] = L"LOGIC";


// config option name 
const wchar_t PORT[] = L"port";
const wchar_t BACKLOG[] = L"backlog";
const wchar_t MAX_WORK_THEAD_CNT[] = L"workerCount";
const wchar_t MAX_SESSION_COUNT[] = L"maxSessionCount";
const wchar_t MAX_IOCP_EVENT_COUNT[] = L"maxIocpEventCount";


// logic config
const wchar_t MAX_CLIENT_COUNT[] = L"maxClientCount";
const wchar_t MAX_ROOM_COUNT[] = L"maxRoomCount";
const wchar_t MAX_ROOM_CLIENT_COUNT[] = L"maxClientCountInRoom";


// 1024 2048 4096 8172
const int RECV_BUFFER_MAX_SIZE = 4096;
const int SEND_BUFFER_MAX_SIZE = 4096;

const int IOCP_EVENT_COUNT = 100;