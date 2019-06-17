#pragma once

#include <atomic>
#include <iostream>
#include <mutex>


// jw - jinwoong
// 자주 쓰는건 추후에 변경의 용이성을 위해 한 곳에 정의

typedef std::recursive_mutex	__lock;
typedef std::thread				__thread;
typedef std::string				__string;
typedef std::wstring			__wstring;

