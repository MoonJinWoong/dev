#pragma once


//TODO : ���߿� �򰥸��� ������ ��Ŷ ���� ���� �ڷ����� ���ϼ� �ְ� ������.
using c_Int = const int;
using u_Int			= unsigned int;
using c_u_Int		= const unsigned int;
using s_Int			= signed int;


using c_Short		= const short;
using c_int		= const unsigned short;


#include <mutex>
using auto_lock = std::lock_guard<std::mutex>;