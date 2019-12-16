#pragma once
#include <string>
#include <fstream>
#include <iostream>

struct Config
{
	void ReadConfig()
	{
		std::ifstream in("../NetLib/config.txt");
	
		std::string buf2;
		while (getline(in, buf2)) 
		{
			ParsingOpt(buf2);
		}
	}

	void ParsingOpt(std::string& str)
	{
		auto token = str.find('=');
		auto optName = str.substr(0, token);
		auto optValue = std::stoi(str.substr(token + 1));

		
		if (optName == "ServerPort")
		{
			mServerPort = optValue;
		}
		else if(optName == "MaxClient")
		{
			mMaxUserCount = optValue;
		}
		else if (optName == "MaxWorkThread")
		{
			mMaxWorkThreadCnt = optValue;
		}
		else if (optName == "ListenWaitQueue")
		{
			mBackLog = optValue;
		}
	}

	int mServerPort = 0;
	int mMaxUserCount = 0;
	unsigned short mMaxWorkThreadCnt = 0;
	unsigned short mBackLog = 0;
};