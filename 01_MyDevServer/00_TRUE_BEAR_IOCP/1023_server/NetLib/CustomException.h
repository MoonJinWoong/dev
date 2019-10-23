#pragma once

#include "NetPreCompile.h"

class CustomException :public std::exception
{
public:
	CustomException(const std::string& msg)
	{
		m_msg = msg + "null";
	}
	CustomException(const std::string& msg , int errCode)
	{
		m_msg = msg + std::to_string(errCode);
	}
	~CustomException() {}

	std::string m_msg;
	int m_code;
	const char* what() { return m_msg.c_str(); }
};