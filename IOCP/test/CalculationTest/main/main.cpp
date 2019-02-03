
#include <iostream>

#include"MathFuncsDll.h"

using namespace std;


int main()
{
	int input1, input2;

	cin >> input1 >> input2;

	cout << MathFuncs::MyMathFuncs::Add(input1, input2) << endl;

}
