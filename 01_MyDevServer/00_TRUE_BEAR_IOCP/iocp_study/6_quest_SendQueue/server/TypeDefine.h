#pragma once


//TODO : 나중에 헷갈릴거 같으니 패킷 만을 위한 자료형을 통일성 있게 만들어보자.
using c_Int = const int;
using u_Int			= unsigned int;
using c_u_Int		= const unsigned int;
using s_Int			= signed int;


using c_Short		= const short;
using c_int		= const unsigned short;


#include <mutex>
using auto_lock = std::lock_guard<std::mutex>;