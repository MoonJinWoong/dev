#pragma once


//TODO : ���߿� �򰥸��� ������ ��Ŷ ���� ���� �ڷ����� ���ϼ� �ְ� ������.
using c_Int = const int;
using u_Int			= unsigned int;
using c_u_Int		= const unsigned int;
using s_Int			= signed int;


using c_Short		= const short;
using u_Short		= unsigned short;
using c_u_Short		= const unsigned short;


#include <mutex>
using auto_lock = std::lock_guard<std::mutex>;