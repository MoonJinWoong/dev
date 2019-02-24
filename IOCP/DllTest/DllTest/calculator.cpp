#include "calculatoe.h"



void Calculator::Sum(int a, int b)
{
	m_Sum = a + b;
}

void Calculator::printSum()
{
	cout << "sum -->"<< m_Sum << endl;
}