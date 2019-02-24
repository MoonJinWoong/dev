#pragma once

#ifndef DLLTEST_EXPORTS
#define DLLTEST_API __declspec(dllexport)
#else
#define DLLTEST_API __declspec(dllimport)
#endif // !DLLTEST_EXPORTS


#include <iostream>

using namespace std;


class DLLTEST_API Calculator
{
private : 
	int m_Sum;
public:
	void Sum(int a, int b);
	void printSum();
};
